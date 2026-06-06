#include <driver/i2s.h>

#define I2S_WS      15
#define I2S_SD      32
#define I2S_SCK     14

#define BUTTON_PIN  4
#define RGB_R       18
#define RGB_B       19
#define RGB_G       21

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512
int32_t i2s_buffer[BUFFER_SIZE]; 

bool isRecording = false;
bool lastButtonState = HIGH;

// 記憶上一次 Gemini 回覆的顏色（預設為微弱藍光就緒）
int currentR = 0;
int currentG = 0;
int currentB = 150;

void setRGB(int r, int g, int b) {
  ledcWrite(RGB_R, 255 - r); 
  ledcWrite(RGB_G, 255 - g);
  ledcWrite(RGB_B, 255 - b);
}

void setupLED() {
  ledcAttach(RGB_R, 5000, 8);
  ledcAttach(RGB_G, 5000, 8);
  ledcAttach(RGB_B, 5000, 8);
}

void initI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, 
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

void setup() {
  Serial.begin(460800); 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setupLED();
  setRGB(currentR, currentG, currentB); 
  initI2S();
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  // 1. 按下錄音：立刻強制切換成「紅色」
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50); 
    isRecording = true;
    setRGB(255, 0, 0); 
    Serial.println("==START_AUDIO==");
  }

  // 2. 放開按鈕：停止錄音並等待 Python 回傳
  if (lastButtonState == LOW && currentButtonState == HIGH) {
    delay(50);
    if (isRecording) {
      isRecording = false;
      Serial.flush(); 
      Serial.println("==END_AUDIO=="); 
      
      // 燈號維持紅燈直到 Python 傳回新顏色

      unsigned long timeout = millis();
      while (millis() - timeout < 8000) { 
        if (Serial.available()) {
          String response = Serial.readStringUntil('\n');
          
          if (response.startsWith("RGB:")) {
            response.replace("RGB:", ""); 
            
            int firstComma = response.indexOf(',');
            int secondComma = response.indexOf(',', firstComma + 1);
            
            if (firstComma != -1 && secondComma != -1) {
              currentR = response.substring(0, firstComma).toInt();
              currentG = response.substring(firstComma + 1, secondComma).toInt();
              currentB = response.substring(secondComma + 1).toInt();
            }
          }
          break; 
        }
        delay(10);
      }
      setRGB(currentR, currentG, currentB); 
    }
  }
  
  lastButtonState = currentButtonState;

  if (isRecording) {
    size_t bytes_read;
    i2s_read(I2S_NUM_0, &i2s_buffer, sizeof(i2s_buffer), &bytes_read, portMAX_DELAY);
    if (bytes_read > 0) {
      Serial.write((uint8_t*)i2s_buffer, bytes_read);
    }
  }
}