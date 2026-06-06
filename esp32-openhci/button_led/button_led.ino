#define BUTTON_PIN  4
#define RGB_R       18
#define RGB_G       21
#define RGB_B       19

// 記錄上一次的按鈕狀態，用來偵測「按下」的那一瞬間（邊緣觸發）
int lastButtonState = HIGH; 

void setRGB(int r, int g, int b) {
  // 假設你是使用共陽極 RGB LED，255 代表全滅，0 代表全亮
  ledcWrite(RGB_R, 255 - r); 
  ledcWrite(RGB_G, 255 - g);
  ledcWrite(RGB_B, 255 - b);
}

void setup() {
  // 初始化按鈕引腳，使用內建上拉電阻（沒按下時是 HIGH，按下時是 LOW）
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  ledcAttach(RGB_R, 5000, 8);
  ledcAttach(RGB_G, 5000, 8);
  ledcAttach(RGB_B, 5000, 8);

  // 初始化隨機數種子（讀取未連接引腳的噪訊，讓每次開機的隨機序列都不同）
  randomSeed(analogRead(0));

  // 初始燈光先調成全滅 (0, 0, 0)
  setRGB(0, 0, 0);
}

void loop() {
  // 讀取當前按鈕狀態
  int currentButtonState = digitalRead(BUTTON_PIN);

  // 偵測按鈕被「按下」的那一瞬間：原本是 HIGH (沒壓)，變成 LOW (按下)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    
    // 生成 0 ~ 255 的隨機 RGB 數值
    int randomR = random(0, 256);
    int randomG = random(0, 256);
    int randomB = random(0, 256);

    // 呼叫設定顏色的函數
    setRGB(randomR, randomG, randomB);

    // 簡單的防彈跳延時 (Debounce)
    delay(50); 
  }

  // 更新按鈕狀態，留到下一次循環比較
  lastButtonState = currentButtonState;
}