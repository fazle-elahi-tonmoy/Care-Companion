#include "HUSKYLENS.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <MAX30105.h>
#include <MPU6050.h>
#include <DFRobotDFPlayerMini.h>
#include "heartRate.h"


HUSKYLENS huskylens;
void printResult(HUSKYLENSResult result);


bool finger = 0;
bool alarm_done = 0;
bool med = 0;
// Wi-Fi credentials
const char* ssid = "CIFT";
const char* password = "cift@1971";

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  // UTC, updates every 60s

// Bluetooth
BluetoothSerial SerialBT;

// Alarm storage
int alarmHour = -1;
int alarmMinute = -1;


// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- GPS ----------
HardwareSerial GPSSerial(2);
#define GPS_RX 25
#define GPS_TX 26
TinyGPSPlus gps;

// ---------- MAX30102 ----------
MAX30105 particleSensor;

// ---------- MPU6050 ----------
MPU6050 mpu;

// ---------- DFPlayer ----------
HardwareSerial DFSerial(1);  // UART1
DFRobotDFPlayerMini myDFPlayer;
#define DF_RX 16
#define DF_TX 17

// ---------- Heart rate smoothing ----------
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute = 0;
int beatAvg = 0;

// ---------- Buzzer ----------
#define BUZZER_PIN 13  // connect active buzzer via transistor (recommended)

// Thresholds for fall detection
float accelThreshold = 1.3;   // g-force threshold for fall detection
float angleThreshold = 80.0;  // degrees tilt threshold

bool fallDetected = false;

// UI toggle (GPS <-> Gyro)
unsigned long lastToggle = 0;
bool showGPS = true;

unsigned long recheck = 0;
unsigned long last_fall = 0;
unsigned long alarm_recheck = 0;
unsigned long med_time = 0;
void setup() {
  Serial.begin(115200);

  // ---------- I2C ----------
  Wire.begin(21, 22);
  SerialBT.begin("Jantra Juddho");  // Bluetooth device name

  // ---------- OLED ----------
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (1) delay(10);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // ---------- GPS ----------
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  // ---------- MAX30102 ----------
  Serial.println("Initializing MAX3010x...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX3010x not found. Check wiring/power.");
    while (1) delay(10);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeIR(0x24);     // IR LED on
  particleSensor.setPulseAmplitudeRed(0x00);    // Red off
  particleSensor.setPulseAmplitudeGreen(0x00);  // Green off
  Serial.println("Place finger on sensor.");

  // ---------- MPU6050 ----------
  mpu.initialize();
  if (!mpu.testConnection()) Serial.println("MPU6050 connection failed");

  // ---------- DFPlayer ----------
  DFSerial.begin(9600, SERIAL_8N1, DF_RX, DF_TX);
  if (!myDFPlayer.begin(DFSerial)) {
    Serial.println("DFPlayer init failed. Check wiring and SD card.");
    while (1) delay(10);
  }
  Serial.println("DFPlayer Mini online.");
  myDFPlayer.volume(40);
  myDFPlayer.play(4);

  // ---------- Buzzer ----------
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  while (!huskylens.begin(Wire)) {
    Serial.println(F("HUSKYLENS not connected!"));
    delay(1000);
  }
  Serial.println("\nWiFi connected");

  // Start NTP
  timeClient.begin();

  // init rates buffer
  for (byte i = 0; i < RATE_SIZE; i++) rates[i] = 0;

  // splash
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("HR+GPS");
  display.setTextSize(1);
  display.println("MPU+DF");
  display.display();
  delay(700);
}

void loop() {
  // ---------- GPS feed ----------
  while (GPSSerial.available()) gps.encode(GPSSerial.read());
  bool gpsValid = gps.location.isValid();
  double lat = gpsValid ? gps.location.lat() : 0.0;
  double lon = gpsValid ? gps.location.lng() : 0.0;

  // ---------- MAX30102 Heart Rate ----------
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60.0 / (delta / 1000.0);

    if (beatsPerMinute > 20 && beatsPerMinute < 255) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      int sum = 0;
      for (byte x = 0; x < RATE_SIZE; x++) sum += rates[x];
      beatAvg = sum / RATE_SIZE;
    }
  }

  // ---------- BPM ALARM ----------
  if (beatAvg > 0 && (beatAvg < 50 || beatAvg > 100) && finger) {
    digitalWrite(BUZZER_PIN, HIGH);  // buzzer ON
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // buzzer OFF
  }

  // ---------- Debug prints ----------
  // Serial.print("IR=");
  // Serial.print(irValue);
  // Serial.print(", BPM=");
  // Serial.print(beatsPerMinute, 1);
  // Serial.print(", Avg BPM=");
  // Serial.print(beatAvg);
  if (irValue < 20000) {
    Serial.print("  No finger?");
    finger = 0;
  } else {
    finger = 1;
  }
  Serial.println();

  // ---------- MPU6050 ----------
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // ---------- OLED UI ----------
  display.clearDisplay();

  // BPM line (centered)
  display.setTextSize(2);
  char bpmLine[16];
  if (irValue >= 20000 && beatAvg > 0) {
    snprintf(bpmLine, sizeof(bpmLine), "BPM:%3d", beatAvg);
  } else {
    snprintf(bpmLine, sizeof(bpmLine), "BPM: --");
  }
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(bpmLine, 0, 0, &x1, &y1, &w, &h);
  int16_t xCentered = (SCREEN_WIDTH - w) / 2;
  display.setCursor(xCentered < 0 ? 0 : xCentered, 0);
  display.print(bpmLine);

  // Toggle subline every 1s
  if (millis() - lastToggle > 1000) {
    lastToggle = millis();
    showGPS = !showGPS;
  }

  display.setTextSize(1);
  display.setCursor(0, 20);
  if (showGPS) {
    if (gpsValid) {
      display.print("Lat:");
      display.print(lat, 5);
      display.print(" Lon:");
      display.print(lon, 5);
    } else {
      display.print("GPS: No fix");
    }
  } else {
    display.print("Gx:");
    display.print(gx);
    display.print(" Gy:");
    display.print(gy);
    display.print(" Gz:");
    display.print(gz);
  }
  fall_detection();
  time();
  display.display();
  delay(10);
}

void fall_detection() {
  if ((millis() - last_fall) > 200) {
    // Read raw accelerometer and gyroscope data
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convert to 'g' force
    float axg = ax / 16384.0;
    float ayg = ay / 16384.0;
    float azg = az / 16384.0;

    // Calculate total acceleration magnitude
    float totalAccel = sqrt(axg * axg + ayg * ayg + azg * azg);

    // Calculate tilt angle (using Z-axis reference)
    float angle = atan2(sqrt(axg * axg + ayg * ayg), azg) * 180.0 / PI;

    // Check fall conditions
    if (totalAccel > accelThreshold && angle < angleThreshold) {
      fallDetected = true;
      Serial.println("⚠️ Fall detected!");
      digitalWrite(BUZZER_PIN, HIGH);  // Alarm ON
      delay(2000);                     // Keep alarm for 2s
      digitalWrite(BUZZER_PIN, LOW);   // Alarm OFF
    } else {
      fallDetected = false;
    }

    // Debug info
    // Serial.print("Accel: ");
    // Serial.print(totalAccel);
    // Serial.print(" g | Angle: ");
    // Serial.print(angle);
    // Serial.println(" deg");
    last_fall = millis();
  }
}

void time() {
  if ((millis() - recheck) > 500) {
    timeClient.update();

    // Display current time for debugging
    int currentHour = timeClient.getHours()+6;
    int currentMinute = timeClient.getMinutes();
    int currentSecond = timeClient.getSeconds();
    Serial.printf("Time: %02d:%02d:%02d\n", currentHour, currentMinute, currentSecond);

    // Check Bluetooth input
    if (SerialBT.available()) {
      String command = SerialBT.readStringUntil('\n');
      command.trim();  // Remove any whitespace

      if (command.startsWith("alarm:-")) {
        String timePart = command.substring(7);  // Get HH:MM
        int separator = timePart.indexOf(':');
        if (separator != -1) {
          alarmHour = timePart.substring(0, separator).toInt();
          alarmMinute = timePart.substring(separator + 1).toInt();
          SerialBT.printf("Alarm set for %02d:%02d\n", alarmHour, alarmMinute);
        }
      }
    }
    // SerialBT.printf("current time %02d:%02d\n", currentHour, currentMinute);
    // Check alarm
    if (alarmHour == currentHour && alarmMinute == currentMinute && currentSecond == 0) {
      SerialBT.println("Please Take Your Prescribed Medicine!");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2000);  // Keep buzzer on for 5 seconds
      digitalWrite(BUZZER_PIN, LOW);
      myDFPlayer.play(1);
      alarm_recheck = millis();
      alarm_done = 1;
    }
    if (((millis() - alarm_recheck) > 30000) && alarm_done == 1 && !med) {
      Serial.println("ALARM! Buzzer ON again!");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(5000);  // Keep buzzer on for 5 seconds
      digitalWrite(BUZZER_PIN, LOW);
      myDFPlayer.play(1);
      alarm_done = 0;
    }
    huskyy();
    recheck = millis();
  }
}

void huskyy() {
  if (!huskylens.request()) {
    Serial.println(F("Fail to request data from HUSKYLENS!"));
  } else if (!huskylens.isLearned()) {
    Serial.println(F("Nothing learned yet!"));
  } else if (!huskylens.available()) {
    Serial.println(F("No tag detected!"));
  } else {
    Serial.println(F("-------- Detected Tags --------"));
    while (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();
      printResult(result);
    }
  }
}


void printResult(HUSKYLENSResult result) {
  if (result.command == COMMAND_RETURN_BLOCK) {
    Serial.print(F("Tag ID: "));
    Serial.println(result.ID);  // Unique ID of the tag
    Serial.print(F("X: "));
    Serial.print(result.xCenter);
    Serial.print(F(", Y: "));
    Serial.println(result.yCenter);
    Serial.print(F("Width: "));
    Serial.print(result.width);
    Serial.print(F(", Height: "));
    Serial.println(result.height);
    if (alarm_done) {
      if (result.ID == 1) {
        med = 1;
        med_time = millis();
        SerialBT.println("Proper Medicine Recognized");
        myDFPlayer.play(3);
      } else {
        if (millis() - med_time > 60000) {
          med = 0;
        }
        SerialBT.println("This medicine is not correct. Please choose the right one");
        myDFPlayer.play(2);
      }
    }
  }
}
