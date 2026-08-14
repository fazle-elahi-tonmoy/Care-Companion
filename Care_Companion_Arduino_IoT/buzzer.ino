void updateBuzzer() {

  unsigned long now = millis();

  if (buzzerPattern == BUZZER_IDLE) {

    digitalWrite(BUZZER_PIN, LOW);
    return;
  }

  if (buzzerPattern == BUZZER_REMINDER) {
    switch (buzzerStep) {
      case 0:
        digitalWrite(BUZZER_PIN, HIGH);
        buzzerTimer = now;
        buzzerStep = 1;
        break;

      case 1:
        if (now - buzzerTimer >= 150) {
          digitalWrite(BUZZER_PIN, LOW);
          buzzerTimer = now;
          buzzerStep = 2;
        }
        break;

      case 2:
        if (now - buzzerTimer >= 100) {
          digitalWrite(BUZZER_PIN, HIGH);
          buzzerTimer = now;
          buzzerStep = 3;
        }
        break;

      case 3:
        if (now - buzzerTimer >= 150) {
          digitalWrite(BUZZER_PIN, LOW);

          buzzerPattern = BUZZER_IDLE;
          buzzerStep = 0;
        }
        break;
    }

    return;
  }

  if (buzzerPattern == BUZZER_WRONG) {

    switch (buzzerStep) {

      case 0:
        digitalWrite(BUZZER_PIN, HIGH);
        buzzerTimer = now;
        buzzerStep = 1;
        break;

      case 1:
        if (now - buzzerTimer >= 2000) {

          digitalWrite(BUZZER_PIN, LOW);

          buzzerPattern = BUZZER_IDLE;
          buzzerStep = 0;
        }
        break;
    }
    return;
  }


  if (buzzerPattern == BUZZER_CORRECT) {
    switch (buzzerStep) {
      case 0:
        // First short beep
        digitalWrite(BUZZER_PIN, HIGH);
        buzzerTimer = now;
        buzzerStep = 1;
        break;

      case 1:
        if (now - buzzerTimer >= 150) {

          digitalWrite(BUZZER_PIN, LOW);
          buzzerTimer = now;
          buzzerStep = 2;
        }
        break;

      case 2:
        // Second short beep
        if (now - buzzerTimer >= 100) {

          digitalWrite(BUZZER_PIN, HIGH);
          buzzerTimer = now;
          buzzerStep = 3;
        }
        break;

      case 3:
        if (now - buzzerTimer >= 150) {

          digitalWrite(BUZZER_PIN, LOW);
          buzzerTimer = now;
          buzzerStep = 4;
        }
        break;

      case 4:
        // Long beep
        if (now - buzzerTimer >= 100) {

          digitalWrite(BUZZER_PIN, HIGH);
          buzzerTimer = now;
          buzzerStep = 5;
        }
        break;

      case 5:
        if (now - buzzerTimer >= 1000) {

          digitalWrite(BUZZER_PIN, LOW);

          buzzerPattern = BUZZER_IDLE;
          buzzerStep = 0;
        }
        break;
    }
  }
}