#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute = 0;
int beatAvg = 0;
unsigned long lastCloudUpdate = 0;
const unsigned long CLOUD_INTERVAL = 10000;

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("Initializing MAX30105...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Check wiring!");
    while (1);
  }
  Serial.println("MAX30105 initialized.");
  Serial.println("Place finger on sensor.");
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60.0 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }


  // Send data to cloud every 60 seconds
  if (millis() - lastCloudUpdate >= CLOUD_INTERVAL) {
    lastCloudUpdate = millis();
    if (irValue > 50000) {
      BPM = beatAvg;
      // SP02 = calculatedSpO2;
      // Temperature = calculatedTemperature;

      Serial.println("================================");
      Serial.println("Sending data to Arduino IoT Cloud");

      Serial.print("Heart Rate: ");
      Serial.print(BPM);
      Serial.println(" BPM");

      Serial.print("SpO2: ");
      Serial.print(SP02);
      Serial.println(" %");

      Serial.print("Temperature: ");
      Serial.print(Temperature);
      Serial.println(" C");

      Serial.println("================================");
    }
    else Serial.println("No finger detected - data not uploaded.");
  }
}


/*
  Since Sp02 is READ_WRITE variable, onSp02Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onSp02Change()  {
  // Add your code here to act upon Sp02 change
}
/*
  Since Bpm is READ_WRITE variable, onBpmChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onBpmChange()  {
  // Add your code here to act upon Bpm change
}
/*
  Since Temperature is READ_WRITE variable, onTemperatureChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTemperatureChange()  {
  // Add your code here to act upon Temperature change
}
/*
  Since Medicine is READ_WRITE variable, onMedicineChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onMedicineChange()  {
  // Add your code here to act upon Medicine change
}
/*
  Since FallDetection is READ_WRITE variable, onFallDetectionChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onFallDetectionChange()  {
  // Add your code here to act upon FallDetection change
}