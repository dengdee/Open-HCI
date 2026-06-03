import socket
import wave
from faster_whisper import WhisperModel

# =========================
# CONFIG
# =========================
HOST = "0.0.0.0"
PORT = 5000

RATE = 16000
CHANNELS = 1
SAMPLE_WIDTH = 2

WAV_FILE = "output.wav"

# =========================
# LOAD MODEL
# =========================
print("Loading faster-whisper model...")

model = WhisperModel(
    "base",
    device="cpu",
    compute_type="int8"
)

print("Model loaded")

# =========================
# TCP SERVER
# =========================
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server.bind((HOST, PORT))
server.listen(1)

print("Waiting ESP32...")

while True:

    conn, addr = server.accept()

    print("Connected:", addr)

    # =========================
    # CREATE WAV
    # =========================
    wav = wave.open(WAV_FILE, "wb")

    wav.setnchannels(CHANNELS)
    wav.setsampwidth(SAMPLE_WIDTH)
    wav.setframerate(RATE)

    print("Receiving audio...")

    # =========================
    # RECEIVE AUDIO
    # =========================
    while True:

        data = conn.recv(1024)

        if data == b"END":
            break

        if not data:
            break

        wav.writeframes(data)

    wav.close()

    print("Audio saved")

    # =========================
    # SPEECH TO TEXT
    # =========================
    print("Transcribing...")

    segments, info = model.transcribe(
        WAV_FILE,
        language="en"
    )

    text = ""

    for segment in segments:
        text += segment.text + " "

    text = text.strip()

    print("\n========== TEXT ==========")
    print(text)
    print("==========================\n")

    # =========================
    # SEND BACK TO ESP32
    # =========================
    try:
        conn.send(text.encode())
        print("Text sent to ESP32")
    except:
        print("Send failed")

    conn.close()