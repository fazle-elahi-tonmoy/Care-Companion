void updateGPS() {
  while (GPS_Serial.available()) {
    char c = GPS_Serial.read();
    gps.encode(c);
  }

  if (gps.location.isUpdated()) {
    if (gps.location.isValid()) {
      gpsLatitude = gps.location.lat();
      gpsLongitude = gps.location.lng();
      gpsValid = true;
      lastGPSFix = millis();
      if (gps.satellites.isValid()) gpsSatellites = gps.satellites.value();
      Serial.print("GPS: ");
      Serial.print(gpsLatitude, 6);
      Serial.print(", ");
      Serial.print(gpsLongitude, 6);
      Serial.print(" | Satellites: ");
      Serial.println(gpsSatellites);
    }
  }
}