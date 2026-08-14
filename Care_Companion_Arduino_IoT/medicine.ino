void checkMedicineSchedule() {
  if (Medicine.isActive()) {
    if (!medicineScanning && !medicineTaken) {
      medicineScanning = true;
      lastMedicineReminder = millis();
      Serial.println();
      Serial.println("================================");
      Serial.println("MEDICATION TIME!");
      Serial.println("================================");
      showMedicineOSD("Take Medicine");
    }

    if (medicineScanning && !medicineTaken && millis() - lastMedicineReminder >= MEDICINE_REMINDER_INTERVAL) {
      lastMedicineReminder = millis();
      Serial.println("Medicine reminder");
      startBuzzer(BUZZER_REMINDER);
      showMedicineOSD("Take Medicine");
    }

    if (medicineScanning && !medicineTaken && millis() - lastMedicineScan >= MEDICINE_SCAN_INTERVAL) {
      lastMedicineScan = millis();
      huskyy();
    }
  }

  else {
    if (medicineScanning || medicineTaken) {
      medicineScanning = false;
      medicineTaken = false;
      buzzerPattern = BUZZER_IDLE;
      digitalWrite(BUZZER_PIN, LOW);
      showMedicineOSD("");
      Serial.println();
      Serial.println("Medicine schedule ended.");
      Serial.println("Ready for next dose.");
    }
  }
}