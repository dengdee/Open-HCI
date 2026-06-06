# 🚀 Open-HCI

## 📌 專案介紹

本專案使用 ESP32 + Python + Gemini + Speech-to-Text 建立互動系統，包含按鈕控制、語音辨識與 AI 控制 RGB LED。

### 📁 ESP32 程式（esp32-openhci/）

- `button_led.ino`  
  👉 按下按鈕後隨機改變 LED 顏色

- `gemini.ino`  
  👉 透過 Serial Monitor 與 Gemini 聊天

- `STT_LLM/`  
  👉 將音訊透過 Serial 傳到 server，用gemini的回覆控制led：

---

### 🖥️ Python Server

- `server.py`
  - 接收 ESP32 音訊
  - 存成 `.wav`
  - 使用 Google Speech-to-Text 進行語音辨識
  - 串接 Gemini 產生回應
  - 控制 LED 顏色

---

# ⚙️ 安裝環境

👉 建議 Python 版本：**>= 3.10**


## 1️⃣ Clone Repository

```bash
git clone https://github.com/<your_username>/<repo_name>.git
cd <repo_name>
```
## 2️⃣ 建立虛擬環境
Windows
```bash
python -m venv venv
venv\Scripts\activate
```
Linux / macOS
```bash
python3 -m venv venv
source venv/bin/activate
```
## 3️⃣ 安裝套件
```bash
pip install -r requirements.txt
```bash

⚠️ FFmpeg（必要）

Windows（winget）：
```bash
winget install Gyan.FFmpeg
```

macOS：
```bash
brew install ffmpeg
```
Linux：
```bash
sudo apt install ffmpeg
```

## 4️⃣ 設定 API Key
Windows (PowerShell)
```bash
$env:GEMINI_API_KEY="你的_GEMINI_API_KEY_字串"
```

macOS / Linux
```bash
export GEMINI_API_KEY="你的_GEMINI_API_KEY_字串"
```

## 5️⃣ 執行 Server
```bash
python server.py
```

## 🔌 ESP32 腳位設定（預設）
程式碼腳位宣告參考

```C++
#define BUTTON_PIN   4

#define RGB_R        18
#define RGB_G        21
#define RGB_B        19

#define I2S_WS       15
#define I2S_SD       32
#define I2S_SCK      14
```



## 📌 備註
🔧 ESP32 驅動程式安裝
如果電腦無法偵測到開發板的序列埠（COM Port），請依據您的作業系統安裝 CH340 晶片驅動：

Windows： [WCH 官方網站](https://www.wch.cn/downloads/CH341SER_EXE.html)。

macOS： [.zip連結](https://drive.google.com/file/d/1dQEHnetZwXCdhG1hbLkvI_ds0QmPThNm/view?usp=drive_link)


