# Smart Air Purifier (ESP32)

A smart air purifier I built with a team of 5 as our 4th-semester mini-project at Ramdeobaba University. It uses an ESP32, PM2.5 and MQ-135 sensors, and adjusts fan speed automatically based on the air quality it reads.

![Smart air purifier — final build](images/01-final-build.png)

## What it does

The setup runs three filters in series — a cloth pre-filter, a HEPA filter, and an activated carbon layer. The ESP32 reads PM2.5 and gas levels continuously, calculates AQI, and uses that to control fan speed through an L298N motor driver. An OLED shows current PM2.5 and AQI values in real time.

![OLED showing live AQI readings](images/02-oled-readings.png)

Live readings during operation — AQI 29 (Good), PM2.5 at 7.1 µg/m³, fan adjusted to 19% based on air quality.

## Results from testing

We tested it in a closed room of around 9–12 m². Starting PM2.5 was about 215 µg/m³, and it dropped to roughly 42 µg/m³ in 10 minutes (about an 80% reduction). Response time to changes in air quality was around 2–3 seconds.

The total build cost was about ₹6,500. Commercial purifiers with similar specs are usually ₹15,000 and up.

## Hardware used

- ESP32 dev board
- GP2Y1010AU0F PM2.5 dust sensor
- MQ-135 gas sensor
- L298N motor driver
- 12V DC fan
- 0.96" OLED display (I2C)
- HEPA + activated carbon filter
- Custom MDF + acrylic enclosure

## Code

Written in Arduino C/C++. The main loop reads sensor values, computes AQI using EPA breakpoints with linear interpolation, maps it to a PWM duty cycle for the fan, and pushes the readings to the OLED. Smoothing buffers reduce sensor noise, and there's a fallback so the system still works on MQ-135 alone if the dust sensor disconnects.

Libraries used: Adafruit_SSD1306, Adafruit_GFX, Wire.

To run it:
1. Open `src/main.ino` in Arduino IDE
2. Install the libraries above
3. Select ESP32 Dev Module as the board
4. Wire up per the wiring shown in `images/`
5. Upload, then open Serial Monitor at 115200 baud

## Build photos

| | |
|---|---|
| ![Internal wiring](images/03-internal-wiring.png) | ![Back view](images/04-back-electronics.png) |
| Internal layout — ESP32, L298N driver, fan, and sensor wiring | Back view showing power supply and electronics |
| ![Filter stages](images/05-filter-stages.png) | ![Assembly](images/06-assembly-process.png) |
| Three-stage filtration: cloth pre-filter → HEPA → carbon | Final assembly on the rooftop |
| ![Workbench](images/07-workbench-setup.png) | ![Team demo](images/08-team-demo.png) |
| Build process — testing the electronics before enclosure | Project demonstration in class |

## What I'd improve

- Add Wi-Fi logging so AQI history can be viewed on a phone
- Calibrate the MQ-135 better — it drifts a bit over time
- Add a proper power switch and indicator LEDs on the enclosure

## Team

Built with 4 teammates under Dr. Alok Kumar Jha. My contribution was mainly on the sensor interfacing, AQI computation logic, and the smoothing/fallback behavior.

## License

MIT
