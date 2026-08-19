void sendSMS(String message) {
  Serial.println("Sending SMS...");

  if (gpsValid) {
    message += "\nMap: https://maps.google.com/?q=";
    message += String(gpsLatitude, 6);
    message += ",";
    message += String(gpsLongitude, 6);
  } else message += "\nGPS location unavailable.";

  SIM800_Serial.println("AT+CMGF=1");
  waiting(500);
  SIM800_Serial.print("AT+CMGS=\"");
  SIM800_Serial.print(SMS_NUMBER);
  SIM800_Serial.println("\"");
  waiting(500);
  SIM800_Serial.print(message);
  SIM800_Serial.write(26);
  waiting(1000);
  Serial.println("SMS sent.");
}

void waiting(int time) {
  uint32_t timing = millis();
  while (millis() - timing < time) {
    gsm_status();
  }
}

void gsm_status() {
  if (SIM800_Serial.available()) {
    char c = SIM800_Serial.read();
    Serial.print(c);
  }
}

void sms_prep() {
  if (step) {
    if (millis() - prep_time > 20000 && step == 1) {
      prep_time = millis();
      SIM800_Serial.println("AT");
      step = 2;
    }

    else if (millis() - prep_time > 1000 && step == 2) {
      SIM800_Serial.println("AT+CMGF=1");
      step = 0;
    }
  }
}