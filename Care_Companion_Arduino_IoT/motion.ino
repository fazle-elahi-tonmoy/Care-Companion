void fall_detection() {
  if (millis() - last_fall > 200) {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float axg = ax / 16384.0;
    float ayg = ay / 16384.0;
    float azg = az / 16384.0;
    float totalAccel = sqrt(axg * axg + ayg * ayg + azg * azg);
    float angle = atan2(sqrt(axg * axg + ayg * ayg), azg) * 180.0 / PI;

    // if (totalAccel > accelThreshold && angle < angleThreshold && !fallDetected) {
    //   Serial.println("Fall detected!");
    //   digitalWrite(BUZZER_PIN, HIGH);  // Alarm ON
    //   sendSMS("FALL DETECTED!");
    //   digitalWrite(BUZZER_PIN, LOW);  // Alarm OFF
    //   Fall_Detection = fallDetected = true;
    //   last_fall_alarm = millis();
    // }

    // Debug info
    // Serial.print("Accel: ");
    // Serial.print(totalAccel);
    // Serial.print(" g | Angle: ");
    // Serial.print(angle);
    // Serial.println(" deg");

    last_fall = millis();
  }
}