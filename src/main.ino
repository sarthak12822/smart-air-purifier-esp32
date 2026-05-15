#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ── Pins ──────────────────────────────────────────────
#define DUST_PIN     36
#define DUST_LED     32
#define MQ_PIN       35
#define IN1          2
#define IN2          15
#define ENA          23
#define RGB_R        33
#define RGB_G        25
#define RGB_B        26

#define PWM_FREQ     1000
#define PWM_RES      8

// ── AQI breakpoints ───────────────────────────────────
struct AQIBreakpoint {
  float pmLow, pmHigh;
  int   aqiLow, aqiHigh;
  const char* label;
};
const AQIBreakpoint BP[] = {
  {  0.0,  12.0,   0,  50, "Good"          },
  { 12.1,  35.4,  51, 100, "Moderate"      },
  { 35.5,  55.4, 101, 150, "Unhlthy(Sens)" },
  { 55.5, 150.4, 151, 200, "Unhealthy"     },
  {150.5, 250.4, 201, 300, "Very Unhlthy"  },
  {250.5, 350.4, 301, 400, "Hazardous"     },
  {350.5, 500.4, 401, 500, "Hazardous+"    }
};
const int BP_COUNT = 7;

// ── MQ135 → approximate AQI mapping ──────────────────
// Raw ADC 0-4095 from MQ135 mapped to AQI range
// These thresholds are based on common MQ135 behavior at 3.3V
int mq135ToAQI(int raw) {
  if (raw < 500)  return 20;   // very clean
  if (raw < 800)  return 45;   // good
  if (raw < 1100) return 75;   // moderate
  if (raw < 1500) return 120;  // unhealthy sensitive
  if (raw < 2000) return 160;  // unhealthy
  if (raw < 2800) return 230;  // very unhealthy
  return 320;                  // hazardous
}

int fanSpeedForAQI(int aqi) {
  if (aqi <= 50)  return  50;
  if (aqi <= 100) return 130;
  if (aqi <= 150) return 200;
  return 255;
}

void setRGB(int r, int g, int b) {
  analogWrite(RGB_R, 255 - r);
  analogWrite(RGB_G, 255 - g);
  analogWrite(RGB_B, 255 - b);
}
void rgbForAQI(int aqi) {
  if      (aqi <= 50)  setRGB(0,   228, 0  );
  else if (aqi <= 100) setRGB(255, 255, 0  );
  else if (aqi <= 150) setRGB(255, 126, 0  );
  else if (aqi <= 200) setRGB(255, 0,   0  );
  else if (aqi <= 300) setRGB(143, 63,  151);
  else                 setRGB(126, 0,   35 );
}

int calcAQI(float pm25) {
  if (pm25 < 0)     pm25 = 0;
  if (pm25 > 500.4) pm25 = 500.4;
  for (int i = 0; i < BP_COUNT; i++) {
    if (pm25 >= BP[i].pmLow && pm25 <= BP[i].pmHigh) {
      float aqi = ((float)(BP[i].aqiHigh - BP[i].aqiLow) /
                   (BP[i].pmHigh - BP[i].pmLow)) *
                  (pm25 - BP[i].pmLow) + BP[i].aqiLow;
      return (int)aqi;
    }
  }
  return 500;
}

int aqiCategory(int aqi) {
  if (aqi <= 50)  return 0;
  if (aqi <= 100) return 1;
  if (aqi <= 150) return 2;
  if (aqi <= 200) return 3;
  if (aqi <= 300) return 4;
  return 5;
}

// ── Dust sensor GP2Y1010AU0F ──────────────────────────
// Returns PM2.5 in ug/m3, or -1 if sensor not connected
float readDust() {
  digitalWrite(DUST_LED, LOW);          // turn IR LED ON
  delayMicroseconds(280);               // wait 280us before reading
  int raw = analogRead(DUST_PIN);
  delayMicroseconds(40);
  digitalWrite(DUST_LED, HIGH);         // turn IR LED OFF
  delayMicroseconds(9680);              // rest of 10ms cycle

  // Detect floating/disconnected sensor
  // A floating GPIO36 reads near 0 or near 4095 randomly
  if (raw < 50 || raw > 4050) return -1;

  float voltage = raw * (3.3f / 4095.0f);

  // GP2Y1010 formula: output 0.9V clean air, ~3.4V at 500ug/m3
  // Sensitivity: 0.5V per 0.1mg/m3
  float pm25 = (voltage - 0.9f) / 0.005f;  // more accurate formula
  return (pm25 < 0) ? 0 : pm25;
}

// ── Smoothing ─────────────────────────────────────────
#define SMOOTH_N 5
float dustBuf[SMOOTH_N] = {0};
int   gasBuf[SMOOTH_N]  = {0};
int   bufIdx = 0;

float smoothDust(float v) {
  // Don't add bad readings to buffer
  if (v < 0) {
    float s = 0;
    for (int i = 0; i < SMOOTH_N; i++) s += dustBuf[i];
    return s / SMOOTH_N;
  }
  dustBuf[bufIdx] = v;
  float s = 0;
  for (int i = 0; i < SMOOTH_N; i++) s += dustBuf[i];
  return s / SMOOTH_N;
}
float smoothGas(int v) {
  gasBuf[bufIdx] = v;
  float s = 0;
  for (int i = 0; i < SMOOTH_N; i++) s += gasBuf[i];
  return s / SMOOTH_N;
}

// ── OLED ──────────────────────────────────────────────
void drawDisplay(int aqi, float pm25, int gasRaw, int fanPWM, bool dustOK) {
  int cat = aqiCategory(aqi);
  display.clearDisplay();

  // ── Big AQI number ────────────────────────────────
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("AQI");

  display.setTextSize(3);
  display.setCursor(28, 0);
  display.print(aqi);

  // ── Category label bar ───────────────────────────
  display.setTextSize(1);
  display.fillRect(0, 25, 128, 11, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(2, 26);
  display.print(BP[cat].label);
  // Show source of AQI on right side of bar
  display.setCursor(90, 26);
  display.print(dustOK ? "DUST" : "GAS");
  display.setTextColor(SSD1306_WHITE);

  // ── PM2.5 row ────────────────────────────────────
  display.setCursor(0, 39);
  if (dustOK) {
    display.print("PM2.5:");
    display.print(pm25, 1);
    display.print("ug");
  } else {
    display.print("PM2.5: --  (no sensor)");
  }

  // ── Gas + Fan row ────────────────────────────────
  display.setCursor(0, 50);
  display.print("Gas:");
  display.print(gasRaw);

  display.setCursor(78, 50);
  display.print("Fan:");
  display.print((int)((fanPWM / 255.0f) * 100));
  display.print("%");

  display.display();
}

// ── Setup ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=============================");
  Serial.println("  Smart Air Purifier BOOT");
  Serial.println("=============================");

  pinMode(DUST_LED, OUTPUT);
  digitalWrite(DUST_LED, HIGH);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  ledcAttach(ENA, PWM_FREQ, PWM_RES);

  // Fan test at boot
  Serial.println("TEST: Fan 100% for 2s...");
  ledcWrite(ENA, 255);
  setRGB(255, 0, 0);
  delay(2000);
  ledcWrite(ENA, 0);
  Serial.println("TEST: Fan OFF");
  delay(300);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED FAILED"));
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.print("Air Purifier v1");
  display.setCursor(10, 35);
  display.print("Starting...");
  display.display();
  delay(1000);
  Serial.println("Boot complete.\n");
}

// ── Loop ──────────────────────────────────────────────
void loop() {
  float rawDust = readDust();          // -1 if sensor bad
  int   rawGas  = analogRead(MQ_PIN);

  bool dustSensorOK = (rawDust >= 0);

  float pm25 = smoothDust(rawDust);
  smoothGas(rawGas);
  bufIdx = (bufIdx + 1) % SMOOTH_N;

  int aqi;

  if (dustSensorOK && pm25 > 0) {
    // ── Dust sensor working: use EPA PM2.5 → AQI ──────
    aqi = calcAQI(pm25);
    // Still boost from gas if very high
    int gasAQI = mq135ToAQI(rawGas);
    if (gasAQI > aqi) aqi = (aqi + gasAQI) / 2;  // blend
  } else {
    // ── Dust sensor not connected: use MQ135 only ─────
    aqi = mq135ToAQI(rawGas);
  }

  int fanPWM = fanSpeedForAQI(aqi);

  if (fanPWM > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  ledcWrite(ENA, fanPWM);

  rgbForAQI(aqi);
  drawDisplay(aqi, pm25, rawGas, fanPWM, dustSensorOK);

  Serial.printf("DustRAW:%.1f PM2.5:%.1f | Gas:%d | AQI:%d [%s] | Fan:%d%%\n",
    rawDust, pm25, rawGas, aqi,
    dustSensorOK ? "DUST+GAS" : "GAS ONLY",
    (int)((fanPWM / 255.0f) * 100));

  delay(1000);
}
