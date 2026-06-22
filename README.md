# 🏭 ESP32 — Industrial Temperature & Vibration Monitor

A real-time monitoring system combining an **ESP32** (C++ firmware) with a **Python backend** to collect, transmit, and store temperature and vibration readings over MQTT.

---

## 📋 Overview

The ESP32 reads a DS18B20 temperature sensor and a vibration sensor every **5 seconds**, publishing the data as JSON to a local MQTT broker. A Python script subscribes to the topic, displays alerts in the terminal, and persists every reading to a SQLite database.

```
┌─────────────────────────────────────────────────────────┐
│                      System Flow                        │
│                                                         │
│  [DS18B20]  ──┐                                         │
│               ├──▶  ESP32  ──MQTT──▶  receiver.py       │
│  [Vibration]──┘      │                    │             │
│            Alert LED  │              SQLite DB           │
│                       │                   │             │
│                       └──────────────  historico.py     │
└─────────────────────────────────────────────────────────┘
```

---

## 📁 Repository Structure

```
ESP32/
├── esp32_monitoramento.ino   # ESP32 firmware (C++ / Arduino)
├── receiver.py               # MQTT subscriber + SQLite persistence
├── historico.py              # Query the last 20 readings from the DB
└── requirements.txt          # Python dependencies
```

---

## ⚙️ Required Hardware

| Component | ESP32 Pin | Description |
|---|---|---|
| DS18B20 sensor | GPIO 4 | Temperature (OneWire protocol) |
| Vibration sensor | GPIO 15 | Digital input |
| Alert LED | GPIO 2 | Turns on when an alert is triggered |
| 4.7kΩ resistor | Between VCC and DATA | Pull-up for the DS18B20 |

---

## 🚀 Getting Started

### 1. ESP32 Firmware

**Required libraries** (install via Arduino IDE or PlatformIO):
- `WiFi.h` (built-in for ESP32)
- `PubSubClient`
- `OneWire`
- `DallasTemperature`

**Set your credentials** in `esp32_monitoramento.ino`:

```cpp
const char* ssid        = "YOUR_WIFI";        // Wi-Fi network name
const char* password    = "YOUR_PASSWORD";    // Wi-Fi password
const char* mqtt_server = "192.168.1.100";    // MQTT broker IP
```

Then compile and upload to the ESP32.

---

### 2. Python Backend

**Prerequisites:**
- Python 3.7+
- A local MQTT broker (e.g. [Mosquitto](https://mosquitto.org/))

**Install dependencies:**

```bash
pip install -r requirements.txt
```

**Start the receiver** (listens to MQTT and saves to the database):

```bash
python receiver.py
```

**Query the reading history:**

```bash
python historico.py
```

---

## 📡 MQTT Protocol

| Parameter | Value |
|---|---|
| Topic | `industria/sensores` |
| Port | `1883` |
| Client ID | `ESP32Monitor` |
| Format | JSON |

**Example payload published by the ESP32:**

```json
{
  "temperatura": 27.4,
  "vibracao": 0
}
```

---

## 🗄️ Database

`receiver.py` automatically creates a `monitoramento.db` SQLite file with the following schema:

```sql
CREATE TABLE leituras (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    data_hora   TEXT,     -- format: YYYY-MM-DD HH:MM:SS
    temperatura REAL,
    vibracao    INTEGER   -- 0 = no vibration, 1 = vibration detected
);
```

---

## 🚨 Alerts

The system triggers alerts under the following conditions:

| Condition | ESP32 Action | Python Output |
|---|---|---|
| Temperature > 35 °C | LED turns on | `ALERTA: TEMPERATURA ALTA` |
| Vibration detected | LED turns on | `ALERTA: VIBRAÇÃO DETECTADA` |
| Normal conditions | LED off | — |

**Example terminal output:**

```
==================================================
NOVA LEITURA
Temperatura: 36.7
Vibração: 0
ALERTA: TEMPERATURA ALTA
==================================================
NOVA LEITURA
Temperatura: 30.1
Vibração: 1
ALERTA: VIBRAÇÃO DETECTADA
```

---

## 🔧 MQTT Broker Setup (Mosquitto)

If you don't have a broker yet, install Mosquitto:

```bash
# Ubuntu/Debian
sudo apt install mosquitto mosquitto-clients
sudo systemctl start mosquitto

# macOS
brew install mosquitto
brew services start mosquitto
```

Make sure the IP address set in the firmware points to the machine running the broker.

---

## 📦 Python Dependencies

```
paho-mqtt
```

> `sqlite3` is part of the Python standard library — no extra install needed.

---
