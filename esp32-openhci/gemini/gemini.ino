
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "csie523";
const char* password = "MakeReality";
const String apiKey = "";  // ⚠️ 請在這裡填入你的 Gemini API Key

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 連接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println("請在 Serial Monitor 輸入要查詢的文字，然後按 Enter (請確認序列埠下拉選單選擇「Both NL & CR」或「New Line」)");
}

void loop() {
  if (Serial.available()) {
    String userText = Serial.readStringUntil('\n');
    userText.trim();
    if (userText.length() == 0) return;

    Serial.println("\n-------------------------------------------");
    Serial.println("Sending: " + userText);

    HTTPClient http;
    http.setReuse(false);  // 每次請求都重新連線，確保連線乾淨
    
    // 使用目前穩定的 gemini-2.0-flash 模型
    String url = "https://generativelanguage.googleapis.com/v1/models/gemini-2.0-flash:generateContent?key=" + apiKey;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // 組成 JSON body，並限制最大輸出 Token 以節省快取
    String payload = "{ \"contents\": [{ \"parts\": [{ \"text\": \"" + userText + "\" }] }], \"generationConfig\": { \"maxOutputTokens\": 300 } }";
    
    int httpCode = http.POST(payload);
    
    if (httpCode > 0) {
      String response = http.getString();
      
      // 【偵錯用】印出完整原始回應，方便排查 API 金鑰或安全攔截問題
      Serial.println("\n[Raw Response From Gemini]:");
      Serial.println(response);
      Serial.println("-------------------------------------------");

      // 使用 ArduinoJson v7 的 JsonDocument（自動管理記憶體大小，防止溢位）
      JsonDocument doc; 
      DeserializationError error = deserializeJson(doc, response);
      
      if (!error) {
        // 檢查 1：API 是否回傳錯誤訊息 (例如 API Key 錯誤、格式錯誤)
        if (doc.containsKey("error")) {
          const char* errMsg = doc["error"]["message"];
          Serial.print("❌ Gemini API 錯誤訊息: ");
          Serial.println(errMsg);
        } 
        // 檢查 2：是否有正常的 candidates 回傳
        else if (doc.containsKey("candidates") && doc["candidates"].size() > 0) {
          // 檢查 3：安全檢查是否阻擋了內容 (例如 finishReason 不是 STOP)
          const char* finishReason = doc["candidates"][0]["finishReason"];
          
          // 讀取生成的文字內容
          const char* generatedText = doc["candidates"][0]["content"]["parts"][0]["text"];
          
          if (generatedText) {
            Serial.println("\n🤖 [Gemini 回應]:");
            Serial.println(generatedText);
          } else {
            Serial.print("⚠️ 未能生成文字。原因 (finishReason): ");
            Serial.println(finishReason ? finishReason : "未知");
          }
        } else {
          Serial.println("⚠️ 找不到 candidates 欄位，請檢查上方 Raw Response 內容。");
        }
      } else {
        Serial.print("❌ JSON 解析失敗: ");
        Serial.println(error.c_str()); 
      }
    } else {
      Serial.print("❌ HTTP 請求失敗，錯誤代碼: ");
      Serial.println(httpCode);
      // 常見錯誤：-1 代表連不到網路或網址不對
    }

    http.end();
    Serial.println("\n請輸入下一個要查詢的文字:");
  }
}