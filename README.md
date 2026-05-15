# Smart Air Purifier 🌬️

An **ESP32-based automated air purification system** with real-time AQI sensing and adaptive fan-speed control. Built as a 4th-semester mini-project at Ramdeobaba University.

---

## 🎯 Key Results

| Metric | Value |
|---|---|
| **PM2.5 reduction** | ~80% (215 → 42 µg/m³) in 10 minutes |
| **Test area** | 9–12 m² |
| **Response latency** | 2–3 seconds |
| **Build cost** | ~₹6,500 |
| **Commercial equivalent cost** | ₹15,000 – ₹50,000+ |

Delivers comparable performance to commercial purifiers at a **fraction of the cost** — making smart purification accessible for households, classrooms, and small offices.

---

## 🛠️ Hardware

- **ESP32** microcontroller
- **PM2.5** dust sensor
- **MQ-135** air quality / gas sensor
- **L298N** motor driver
- **12V DC fan** (PWM-controlled)
- **OLED display** (I2C, 0.96")
- **3-stage filtration:** cloth pre-filter → HEPA → activated carbon

## ⚡ Features

- 📊 Real-time AQI computation from PM2.5 + gas readings
- 🌀 Adaptive PWM-based fan-speed control (faster fan in worse air)
- 🖥️ Live OLED readout of PM2.5 levels and AQI
- 💸 Low-cost, fully open hardware design
- 🔌 Modular sensor architecture — easy to extend

## 🧠 How it works

1. Sensors continuously sample air quality (PM2.5 + gas concentrations)
2. ESP32 computes AQI in real time
3. PWM signal adjusts fan speed based on AQI thresholds
4. OLED displays current PM2.5 and AQI to the user
5. Multi-stage filter physically removes particulates and odors

## 📂 Repository structure

- `src/` — Arduino C/C++ source code
- `hardware/` — Circuit diagrams, BOM, wiring
- `docs/` — Project report, references
- `images/` — Build photos, demo screenshots

## 🚀 Getting started

1. Clone this repo
2. Open `src/main.ino` in **Arduino IDE**
3. Install required libraries: `Adafruit_SSD1306`, `Adafruit_GFX`, `Wire`
4. Select board: **ESP32 Dev Module**
5. Wire up the circuit per `hardware/circuit-diagram.png`
6. Upload and monitor via Serial @ 115200 baud

## 👥 Team & Acknowledgements

Built with a team of 5 under the guidance of **Dr. Alok Kumar Jha**, Ramdeobaba University.

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.

---

> Built with ❤️ in Nagpur · 4th Semester Mini Project · Ramdeobaba University
