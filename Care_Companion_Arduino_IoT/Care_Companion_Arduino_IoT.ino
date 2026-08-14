#include "HUSKYLENS.h"
#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
MAX30105 particleSensor;
HUSKYLENS huskylens;

#define BUZZER_PIN 4
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
const float TEMPERATURE_OFFSET_F = 11.5;

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
  BUZZER_CORRECT
};
BuzzerPattern buzzerPattern = BUZZER_IDLE;
unsigned long buzzerTimer = 0;
int buzzerStep = 0;
bool buzzerActive = false;

void startBuzzer(BuzzerPattern pattern) {
  buzzerPattern = pattern;
  buzzerStep = 0;
  buzzerTimer = millis();
  buzzerActive = false;
  digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.begin(115200);
  delay(1500);
  Serial.println("Initializing MAX30105...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Check wiring!");
    while (1)
      ;
  }
  while (!huskylens.begin(Wire)) {
    Serial.println(F("HUSKYLENS not connected!"));
    while (1)
      ;
  }
  Serial.println("MAX30105 initialized.");
  Serial.println("Place finger on sensor.");
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x24);
  particleSensor.setPulseAmplitudeGreen(0);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  processSpO2Sample();
  processHeartRate();
  checkMedicineSchedule();
  updateBuzzer();

  if (millis() - lastCloudUpdate >= CLOUD_INTERVAL) {
    lastCloudUpdate = millis();
    long irValue = particleSensor.getIR();
    if (irValue > 50000) {
      if (beatAvg > 20) BPM = beatAvg;
      if (validSpO2 && spo2 >= 70 && spo2 <= 100) SP02 = spo2;
      Temperature = readTemperatureF();

      Serial.println("=================================");
      Serial.print("Heart Rate : ");
      Serial.print(BPM);
      Serial.println(" BPM");
      Serial.print("SpO2       : ");
      Serial.print(SP02);
      Serial.println(" %");
      Serial.print("Temperature: ");
      Serial.print(Temperature);
      Serial.println(" °F");
      Serial.println("=================================");
    }

    else Serial.println("No finger detected.");
  }
}




/*
  Since Sp02 is READ_WRITE variable, onSp02Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onSp02Change() {
  // Add your code here to act upon Sp02 change
}
/*
  Since Bpm is READ_WRITE variable, onBpmChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onBpmChange() {
  // Add your code here to act upon Bpm change
}
/*
  Since Temperature is READ_WRITE variable, onTemperatureChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTemperatureChange() {
  // Add your code here to act upon Temperature change
}
/*
  Since Medicine is READ_WRITE variable, onMedicineChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onMedicineChange() {
  // Add your code here to act upon Medicine change
}
/*
  Since FallDetection is READ_WRITE variable, onFallDetectionChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onFallDetectionChange() {
  // Add your code here to act upon FallDetection change
}
