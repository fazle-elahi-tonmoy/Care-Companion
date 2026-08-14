float readTemperatureF() {
  float temperatureC = particleSensor.readTemperature();
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  temperatureF += TEMPERATURE_OFFSET_F;
  return temperatureF;
}

void processSpO2Sample() {
  particleSensor.check();
  while (particleSensor.available()) {
    uint32_t irValue = particleSensor.getFIFOIR();
    uint32_t redValue = particleSensor.getFIFORed();
    irBuffer[spo2SampleCount] = irValue;
    redBuffer[spo2SampleCount] = redValue;
    spo2SampleCount++;
    particleSensor.nextSample();
    if (spo2SampleCount >= SPO2_BUFFER_SIZE) {
      maxim_heart_rate_and_oxygen_saturation(irBuffer, SPO2_BUFFER_SIZE, redBuffer, &spo2, &validSpO2, &algorithmHeartRate, &validHeartRate);
      Serial.println();
      Serial.println("--------- SENSOR RESULT ---------");
      if (validSpO2) {
        Serial.print("SpO2: ");
        Serial.print(spo2);
        Serial.println(" %");
      } else Serial.println("SpO2: Invalid");

      if (validHeartRate) {
        Serial.print("Algorithm HR: ");
        Serial.print(algorithmHeartRate);
        Serial.println(" BPM");
      } else Serial.println("Algorithm HR: Invalid");
      Serial.println("---------------------------------");
      spo2SampleCount = 0;
    }
  }
}

void processHeartRate() {
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60.0 / (delta / 1000.0);
    if (beatsPerMinute < 255 &&
        beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
}

