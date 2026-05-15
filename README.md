# Smart Air Purifier (ESP32)

A smart air purifier I built with a team of 5 as our 4th-semester mini-project at Ramdeobaba University. It uses an ESP32, PM2.5 and MQ-135 sensors, and adjusts fan speed automatically based on the air quality it reads.

## What it does

The setup runs three filters in series — a cloth pre-filter, a HEPA filter, and an activated carbon layer. The ESP32 reads PM2.5 and gas levels continuously, calculates AQI, and uses that to control fan speed through an L298N motor driver. An OLED shows current PM2.5 and AQI values in real time.

## Results from testing

We tested it in a closed room of around 9–12 m². Starting PM2.5 was about 215 µg/m³, and it dropped to roughly 42 µg/m³ in 10 minutes (about an 80% reduction). Response time to changes in air quality was around 2–3 seconds.

The total build cost was about ₹6,500. Commercial purifiers with similar specs are usually ₹15,000 and up.

## Hardware used

- ESP32 dev board
- PM2.5 dust sensor
- MQ-135 gas sensor
- L298N motor driver
- 12V DC fan
- 0.96" OLED display (I2C)
- HEPA + activated carbon filter

## Code

Written in Arduino C/C++. The main loop reads sensor values, computes AQI, maps it to a PWM duty cycle for the fan, and pushes the readings to the OLED.

Libraries used: Adafruit_SSD1306, Adafruit_GFX, Wire.

To run it:
1. Open `src/main.ino` in Arduino IDE
2. Install the libraries above
3. Select ESP32 Dev Module as the board
4. Wire up per the diagram in `hardware/`
5. Upload, then open Serial Monitor at 115200 baud

## What I'd improve

- Add Wi-Fi logging so AQI history can be viewed on a phone
- Calibrate the MQ-135 better — it drifts a bit over time
- Use a proper enclosure (we used cardboard for the prototype)

## Team

Built with 5 teammates under Dr. Alok Kumar Jha. My contribution was mainly on the sensor interfacing and AQI computation logic.

## License

MIT
