#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// 1. 設定你的 Wi-Fi 帳號密碼
const char* ssid = "csie523";
const char* password = "MakeReality";

// 2. 設定你的 Gemini API Key
const char* apiKey = "";

// Gemini API URL
const String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(apiKey);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 連接 Wi-Fi
  Serial.println("\n=================================");
  Serial.print("正在連接到 Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi 連線成功！");
  Serial.print("IP 位址: ");
  Serial.println(WiFi.localIP());
  Serial.println("=================================");
  Serial.println("【Gemini 聊天室已就緒】");
  printPrompt();
}

void loop() {
  // 檢查序列埠是否有資料進來
  if (Serial.available() > 0) {
    // 讀取輸入直到換行符號 (\n)
    String input = Serial.readStringUntil('\n');
    
    // 清除前後空白與換行符號
    input.trim(); 
    
    // 如果輸入不是空的，就發送給 Gemini
    if (input.length() > 0) {
      Serial.print("你說: ");
      Serial.println(input);
      
      // 呼叫 Gemini API
      sendToGemini(input);
      
      // 結束後提示可以繼續輸入
      printPrompt();
    }
  }
}

// 提示輸入的小圖示
void printPrompt() {
  Serial.println("\n---------------------------------");
  Serial.print("請輸入你想對 Gemini 說的話：\n> ");
}

// 發送請求給 Gemini API 的函式
void sendToGemini(String prompt) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // 建立 JSON 結構
    JsonDocument doc;
    JsonArray contents = doc.createNestedArray("contents");
    JsonObject contentObj = contents.createNestedObject();
    JsonArray parts = contentObj.createNestedArray("parts");
    JsonObject partObj = parts.createNestedObject();
    partObj["text"] = prompt;

    String requestBody;
    serializeJson(doc, requestBody);

    Serial.println("Gemini 正在思考中...");
    
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      
      JsonDocument responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        const char* reply = responseDoc["candidates"][0]["content"]["parts"][0]["text"];
        if (reply) {
          Serial.println("\nGemini 回應：");
          Serial.println(reply);
        } else {
          Serial.println("【系統錯誤】無法解析 Gemini 的回應內容。");
        }
      } else {
        Serial.print("【系統錯誤】JSON 解析失敗: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("【系統錯誤】連線失敗，HTTP 錯誤代碼: ");
      Serial.println(httpResponseCode);
    }
    
    http.end(); 
  } else {
    Serial.println("【系統錯誤】Wi-Fi 連線已中斷，請檢查網路。");
  }
}