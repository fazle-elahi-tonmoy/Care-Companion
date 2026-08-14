void showMedicineOSD(const char *message) {
  huskylens.customText("                    ", 160, 30);
  if (strlen(message) > 0) huskylens.customText(message, 160, 30);
}

void huskyy() {
  if (!huskylens.request()) {
    Serial.println(F("Fail to request data from HUSKYLENS!"));
    return;
  }
  if (!huskylens.isLearned()) {
    Serial.println(F("Nothing learned yet!"));
    return;
  }
  if (!huskylens.available()) {
    Serial.println(F("No medicine detected!"));
    return;
  }

  while (huskylens.available()) {
    HUSKYLENSResult result = huskylens.read();
    processMedicineTag(result);
  }
}

void processMedicineTag(HUSKYLENSResult result) {
  if (result.command != COMMAND_RETURN_BLOCK) {
    return;
  }
  Serial.print("Medicine Tag ID: ");
  Serial.println(result.ID);

  if (result.ID == 1) {
    Serial.println("RIGHT MEDICINE!");
    medicineTaken = true;
    medicineScanning = false;
    lastMedicineReminder = millis();
    showMedicineOSD("Right Medicine");
    startBuzzer(BUZZER_CORRECT);
    return;
  }

  Serial.println("WRONG MEDICINE!");
  showMedicineOSD("Wrong Medicine");
  if (millis() - lastWrongMedicine >= WRONG_MEDICINE_INTERVAL) {
    lastWrongMedicine = millis();
    startBuzzer(BUZZER_WRONG);
  }
}