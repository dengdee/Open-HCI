# Open-HCI

## 安裝

### 1. Clone Repository

```bash
git clone https://github.com/<your_username>/<repo_name>.git
cd <repo_name>
```

### 2. 建立虛擬環境

Windows：

```bash
python -m venv venv
venv\Scripts\activate
```

Linux：

```bash
python3 -m venv venv
source venv/bin/activate
```

### 3. 安裝套件

```bash
pip install -r requirements.txt
winget install Gyan.FFmpeg
```

### 4.設定apikey

```bash
$env:GEMINI_API_KEY="你的_GEMINI_API_KEY_字串"

```

### 5.執行server

```bash
python server.py

```

### 5.Vibe Coding
可以將以下貼給ai
```bash
#define BUTTON_PIN  4
#define RGB_R       18
#define RGB_G       21
#define RGB_B       19

#define I2S_WS      15
#define I2S_SD      32
#define I2S_SCK     14

```


---