void handlePanicEvent() {
  if (panicEvent) {
    panicEvent = false;
    buzzerPattern = BUZZER_PANIC;
    if (!panicActive) {
      panicActive = true;
      Serial.println();
      Serial.println("================================");
      Serial.println("!!! PANIC EVENT TRIGGERED !!!");
      Serial.println("================================");
      sendSMS("PANIC ALERT!");
      waiting(5000);
      SIM800_Serial.print("ATD01685696928;");
      SIM800_Serial.write(13);
      Fall_Detection = true;
      lastPanicSMS = last_fall_alarm = millis();
    }
  }

  if (panicActive) {
    if (millis() - lastPanicSMS >= PANIC_SMS_INTERVAL) {
      lastPanicSMS = millis();
      Serial.println("Sending panic location update...");
      sendSMS("PANIC ALERT!");
    }
  }
}