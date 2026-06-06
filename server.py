import os
import serial
import wave
import speech_recognition as sr
from pydub import AudioSegment
from google import genai
from google.genai import types
from pydantic import BaseModel, Field

# --- 設定區 ---
COM_PORT = 'COM4' # 改成esp32的port
BAUD_RATE = 460800
WAVE_OUTPUT_FILENAME = "serial_audio.wav"
STT_TEMP_FILENAME = "serial_audio_stt.wav"

# 從環境變數讀取 API KEY
GEMINI_API_KEY = os.environ.get("GEMINI_API_KEY")
if not GEMINI_API_KEY:
    raise ValueError("❌ 錯誤：找不到環境變數 GEMINI_API_KEY，請參考 README.md 設定。")

# --- Pydantic 結構化定義（強制 Gemini 回傳此 JSON 格式） ---
class RGBColor(BaseModel):
    r: int = Field(description="紅色通道值，範圍 0-255")
    g: int = Field(description="綠色通道值，範圍 0-255")
    b: int = Field(description="藍色通道值，範圍 0-255")
    reason: str = Field(description="為什麼選擇這個顏色的簡短理由（繁體中文）")

def ask_gemini_for_color(text: str) -> tuple:
    """將文字送給 Gemini，並強制要求回傳 RGB 數值"""
    try:
        client = genai.Client(api_key=GEMINI_API_KEY)
        prompt = f"分析使用者的話語中所代表的情緒、意境、或是明確提到的顏色，並分派一個最適合的 RGB 燈光顏色。使用者說的話：「{text}」"
        
        response = client.models.generate_content(
            model='gemini-2.5-flash',
            contents=prompt,
            config=types.GenerateContentConfig(
                response_mime_type="application/json",
                response_schema=RGBColor,
                temperature=0.2,
            ),
        )
        
        import json
        result = json.loads(response.text)
        print(f"🤖 Gemini 思考理由: {result.get('reason')}")
        return result.get('r', 0), result.get('g', 0), result.get('b', 0)
        
    except Exception as e:
        print(f"⚠️ Gemini API 呼叫失敗: {e}")
        return 0, 0, 150

def start_serial_server():
    print(f"📡 正在開啟序列埠 {COM_PORT}...")
    try:
        ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1)
    except Exception as e:
        print(f"❌ 無法開啟 {COM_PORT}。請確認 Arduino IDE 的「序列埠監視器」是否已關閉！\n錯誤: {e}")
        return

    print(f"✅ {COM_PORT} 已成功連線！等待 ESP32 按下按鈕...")
    print("-" * 50)
    recognizer = sr.Recognizer()

    try:
        while True:
            if ser.in_waiting > 0:
                line = ser.readline()
                if b"==START_AUDIO==" in line:
                    print("\n🎙️ 錄音中...")
                    wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
                    wf.setnchannels(1)
                    wf.setsampwidth(4) # 32-bit
                    wf.setframerate(16000)
                    
                    raw_audio_buffer = b""
                    while True:
                        if ser.in_waiting > 0:
                            data = ser.read(ser.in_waiting)
                            raw_audio_buffer += data
                            if b"==END_AUDIO==" in raw_audio_buffer:
                                parts = raw_audio_buffer.split(b"==END_AUDIO==")
                                wf.writeframes(parts[0])
                                print("⏹️ 停止錄音！")
                                break
                            if len(raw_audio_buffer) > 30:
                                wf.writeframes(raw_audio_buffer[:-30])
                                raw_audio_buffer = raw_audio_buffer[-30:]
                    wf.close()
                    
                    stt_text = ""
                    if os.path.exists(WAVE_OUTPUT_FILENAME) and os.path.getsize(WAVE_OUTPUT_FILENAME) > 44:
                        try:
                            sound = AudioSegment.from_file(WAVE_OUTPUT_FILENAME, format="wav")
                            sound = sound.set_sample_width(2) # 降轉 16-bit
                            sound.export(STT_TEMP_FILENAME, format="wav")

                            with sr.AudioFile(STT_TEMP_FILENAME) as source:
                                audio_data = recognizer.record(source)
                                stt_text = recognizer.recognize_google(audio_data, language="zh-TW")
                                print(f"🎯 說話內容: 「{stt_text}」")
                        except Exception as ex:
                            print(f"⚠️ STT 失敗: {ex}")
                        finally:
                            if os.path.exists(STT_TEMP_FILENAME):
                                os.remove(STT_TEMP_FILENAME)
                    
                    if stt_text:
                        r, g, b = ask_gemini_for_color(stt_text)
                        print(f"🎨 決定分派顏色 -> R:{r}, G:{g}, B:{b}")
                        response_msg = f"RGB:{r},{g},{b}\n"
                        ser.write(response_msg.encode('utf-8'))
                    else:
                        ser.write(b"RGB:0,0,150\n")
                        
                    print("-" * 50)

    except KeyboardInterrupt:
        print("\n🛑 關閉 Server")
    finally:
        ser.close()

if __name__ == "__main__":
    start_serial_server()