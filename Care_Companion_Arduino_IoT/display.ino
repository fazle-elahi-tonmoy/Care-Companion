void text(String text, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (128 - w) / 2;
  display.setCursor(x, y);
  display.print(text);
}

void display_status() {
  String bpm, SpO2 = "SPO2 ", Temp_s;
  if (millis() - disp_update > 1000) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SAT: ");
    display.print(gpsSatellites);
    if (conn_status) display.drawBitmap(110, 0, epd_bitmap_network_16x8, 16, 8, WHITE);
    display.setTextSize(2);

    long irValue = particleSensor.getIR();
    if (irValue > 50000) {
      (beatAvg > 20) ? bpm = String(beatAvg) + " bpm" : bpm = "---";
      (validSpO2 && spo2 >= 70 && spo2 <= 100) ? SpO2 += String(spo2) + "%" : SpO2 = "---";
      Temperature = readTemperatureF();
      Temp_s = String(Temperature, 2) + String((char)247) + "C";
      text(bpm, 12);
      text(SpO2, 30);
      text(Temp_s, 48);
      // display.print((char)247);
      // display.print("C");
    } else {
      text("No", 18);
      text("Finger", 44);
    }
    display.display();
    disp_update = millis();
  }
}