#define DEBUG
#define GPS_RX 17
#define GPS_TX 16
#include <TinyGPS++.h>
#include <Adafruit_SSD1306.h>
HardwareSerial GPS_Serial(2);
TinyGPSPlus gps;
Adafruit_SSD1306 display(128, 64, &Wire);

void setup() {
  Serial.begin(115200);
  GPS_Serial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("GPS Initializing...");
  display.display();
}

void loop() {

  // Read all available GPS data
  while (GPS_Serial.available()) {
    char c = GPS_Serial.read();
#ifdef DEBUG
    Serial.write(c);
#endif
    gps.encode(c);
  }

  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate >= 1000) {
    lastDisplayUpdate = millis();
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("BN220 GPS");
    display.setTextSize(2);
    display.setCursor(0, 18);
    display.print("SAT: ");
    (gps.satellites.isValid()) ? display.println(gps.satellites.value()) : display.println("--");
    display.setTextSize(1);
    display.setCursor(0, 45);
    (gps.location.isValid()) ? display.println("GPS FIX: YES") : display.println("GPS FIX: NO");
    display.display();
  }

#ifdef DEBUG
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug >= 5000) {
    lastDebug = millis();
    Serial.println();
    Serial.println("--- GPS STATUS ---");
    Serial.print("Satellites: ");

    (gps.satellites.isValid()) ? Serial.println(gps.satellites.value()) : Serial.println("INVALID");

    Serial.print("Location: ");
    if (gps.location.isValid()) {
      Serial.print(gps.location.lat(), 6);
      Serial.print(", ");
      Serial.println(gps.location.lng(), 6);
    }

    else Serial.println("INVALID");
    Serial.print("HDOP: ");
    (gps.hdop.isValid()) ? Serial.println(gps.hdop.hdop()) : Serial.println("INVALID");
    Serial.println("------------------");
  }
#endif
}