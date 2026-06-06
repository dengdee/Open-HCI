# 🚀 Open-HCI

## 📌 專案介紹

本專案使用 ESP32 + Python + Gemini + Speech-to-Text 建立互動系統，包含按鈕控制、語音辨識與 AI 控制 RGB LED。

### 📁 ESP32 程式（esp32-openhci/）

- `button_led.ino`  
  👉 按下按鈕後隨機改變 LED 顏色

- `gemini.ino`  
  👉 透過 Serial Monitor 與 Gemini 聊天

- `STT_LLM/`  
  👉 將音訊透過 Serial 傳到 server：
  1. server 接收音訊
  2. 轉成 WAV
  3. Google STT 轉文字
  4. 餵給 Gemini 生成回覆
  5. 控制 RGB LED 顏色

---

### 🖥️ Python Server

- `server.py`
  - 接收 ESP32 音訊
  - 存成 `.wav`
  - 使用 Google Speech-to-Text 進行語音辨識
  - 串接 Gemini 產生回應

---

# ⚙️ 安裝環境

👉 建議 Python 版本：**>= 3.10**

---

## 1️⃣ Clone Repository

```bash
git clone https://github.com/<your_username>/<repo_name>.git
cd <repo_name>
```
## 2️⃣ 建立虛擬環境
Windows
python -m venv venv
venv\Scripts\activate
Linux / macOS
python3 -m venv venv
source venv/bin/activate
## 3️⃣ 安裝套件
pip install -r requirements.txt
⚠️ FFmpeg（必要）

Windows（winget）：

winget install Gyan.FFmpeg

macOS：

brew install ffmpeg

Linux：

sudo apt install ffmpeg
## 4️⃣ 設定 API Key
Windows (PowerShell)
$env:GEMINI_API_KEY="你的_GEMINI_API_KEY_字串"
macOS / Linux
export GEMINI_API_KEY="你的_GEMINI_API_KEY_字串"

## 5️⃣ 執行 Server
python server.py

## 🔌 ESP32 腳位設定（預設）請依照以下定義將電路元件連接至 ESP32 開發板：功能元件程式內定義 (Define)建議連接腳位 (GPIO)按鈕BUTTON_PIN4RGB LED (紅)RGB_R18RGB LED (綠)RGB_G21RGB LED (藍)RGB_B19I2S 麥克風 (WS)I2S_WS15I2S 麥克風 (SD)I2S_SD32I2S 麥克風 (SCK)I2S_SCK14📌 程式碼腳位宣告參考

```C++

#define BUTTON_PIN   4

#define RGB_R        18
#define RGB_G        21
#define RGB_B        19

#define I2S_WS       15
#define I2S_SD       32
#define I2S_SCK      14
```



📌 備註
🔧 ESP32 驅動程式安裝
如果電腦無法偵測到開發板的序列埠（COM Port），請依據您的作業系統安裝 CH340 晶片驅動：

Windows： 請至 WCH 官方網站下載最新版 CH340 驅動。

macOS： 請透過下方備用連結下載並安裝：

👉 macOS CH340 驅動下載 (Google Drive)