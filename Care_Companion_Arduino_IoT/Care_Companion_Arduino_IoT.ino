#include "arduino_secrets.h"
#include "thingProperties.h"
#include "HUSKYLENS.h"
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
#include <MPU6050.h>
#include <TinyGPSPlus.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire);
MAX30105 particleSensor;
HUSKYLENS huskylens;
MPU6050 mpu;
TinyGPSPlus gps;

HardwareSerial GPS_Serial(1);
HardwareSerial SIM800_Serial(2);


#define BUZZER_PIN 4
#define PANIC_BUTTON_PIN 27
#define GPS_RX_PIN 17  // ESP32 RX <- BN220 TX
#define GPS_TX_PIN 16  // ESP32 TX -> BN220 RX
#define SIM800_RX_PIN 25
#define SIM800_TX_PIN 26
#define SMS_NUMBER "01685696928"


volatile bool panicEvent = false;
bool panicActive = false;
unsigned long lastPanicSMS = 0;
const unsigned long PANIC_SMS_INTERVAL = 60000UL;
unsigned long prep_time;
byte step = 1;
int conn_status;


double gpsLatitude = 0.0;
double gpsLongitude = 0.0;
int gpsSatellites = 0;
bool gpsValid = false;
unsigned long lastGPSFix = 0;

float accelThreshold = 2;     // g-force threshold for fall detection
float angleThreshold = 80.0;  // degrees tilt threshold
bool fallDetected = false;
unsigned long last_fall = 0, last_fall_alarm = 0;

unsigned long rightMedicineOSDTime = 0;
bool rightMedicineOSDActive = false;
const unsigned long RIGHT_MEDICINE_OSD_DURATION = 2000;
uint32_t disp_update;

#define SPO2_BUFFER_SIZE 100
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute = 0;
int beatAvg = 0;
uint32_t irBuffer[SPO2_BUFFER_SIZE];
uint32_t redBuffer[SPO2_BUFFER_SIZE];
int32_t spo2;
int8_t validSpO2;
int32_t algorithmHeartRate;
int8_t validHeartRate;
int spo2SampleCount = 0;
const float TEMPERATURE_OFFSET_F = 5.5;

unsigned long lastCloudUpdate = 0;
const unsigned long CLOUD_INTERVAL = 10000;
unsigned long lastMedicineScan = 0;
const unsigned long MEDICINE_SCAN_INTERVAL = 300;
unsigned long lastMedicineReminder = 0;
const unsigned long MEDICINE_REMINDER_INTERVAL = 5000;
unsigned long lastWrongMedicine = 0;
const unsigned long WRONG_MEDICINE_INTERVAL = 2000;

bool medicineScanning = false;
bool medicineTaken = false;

enum BuzzerPattern {
  BUZZER_IDLE,
  BUZZER_REMINDER,
  BUZZER_WRONG,
  BUZZER_CORRECT,
  BUZZER_PANIC
};

BuzzerPattern buzzerPattern = BUZZER_IDLE;
unsigned long buzzerTimer = 0;
int buzzerStep = 0;
bool buzzerActive = false;

void IRAM_ATTR panicButtonISR() {
  panicEvent = true;
}

void startBuzzer(BuzzerPattern pattern) {
  buzzerPattern = pattern;
  buzzerStep = 0;
  buzzerTimer = millis();
  buzzerActive = false;
  digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  pinMode(PANIC_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PANIC_BUTTON_PIN), panicButtonISR, FALLING);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.begin(115200);
  GPS_Serial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  SIM800_Serial.begin(9600, SERIAL_8N1, SIM800_RX_PIN, SIM800_TX_PIN);
  delay(100);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  display.setTextSize(2);
  display.setTextColor(1);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Check wiring!");
    text("SENSOR", 18);
    text("FAILED", 44);
    display.display();
    while (1)
      ;
  }
  while (!huskylens.begin(Wire)) {
    Serial.println(F("HUSKYLENS not connected!"));
    text("CAMERA", 18);
    text("FAILED", 44);
    display.display();
    while (1)
      ;
  }

  mpu.initialize();
  delay(100);
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    text("GYRO", 18);
    text("FAILED", 44);
    display.display();
    while (1)
      ;
  }

  
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x24);
  particleSensor.setPulseAmplitudeGreen(0);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  prep_time = millis();
}

void loop() {
  sms_prep();
  ArduinoCloud.update();
  processHeartRate();
  checkMedicineSchedule();
  fall_detection();
  updateBuzzer();
  updateMedicineOSD();
  updateGPS();
  handlePanicEvent();
  gsm_status();
  display_status();

  if (millis() - lastCloudUpdate >= CLOUD_INTERVAL) {
    lastCloudUpdate = millis();
    long irValue = particleSensor.getIR();
    if (irValue > 50000) {
      if (beatAvg > 20) BPM = beatAvg;
      if (validSpO2 && spo2 >= 70 && spo2 <= 100) SP02 = spo2;
      Temperature = readTemperatureF();
    }
    conn_status = ArduinoCloud.connected();
    // Serial.println("=================================");
    // Serial.print("Heart Rate : ");
    // Serial.print(BPM);
    // Serial.println(" BPM");
    // Serial.print("SpO2       : ");
    // Serial.print(SP02);
    // Serial.println(" %");
    // Serial.print("Temperature: ");
    // Serial.print(Temperature);
    // Serial.println(" °F");
    // Serial.print("Warning: ");
    // Serial.println((Fall_Detection) ? "True" : "False");
    // Serial.print("Connection: ");
    // Serial.println((conn_status) ? "Connected" : "Not Link");
    // Serial.println("=================================");
  }

  if (millis() - last_fall_alarm > 10000 && Fall_Detection)
    Fall_Detection = fallDetected = false;

  if (Serial.available()) {
    String c = Serial.readStringUntil('\n');
    if (c.startsWith("RST")) ESP.restart();
    SIM800_Serial.print(c);
  }
}
