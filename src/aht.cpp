/******************************************************************
 *                      AHT10 Sensor
 *                      қuran nov 2022
 ******************************************************************/

 #include <Arduino.h>
 #include <Wire.h>
 #include "aht.h"
 
 #define AHT10_ADDR 0x38  // I2C-Adresse des AHT10 Sensors
 
 void initAHT(void)
 {
     Wire.beginTransmission(AHT10_ADDR);
     Wire.write(0xE1);  // Initialisierungsbefehl
     Wire.write(0x08);
     Wire.write(0x00);
     Wire.endTransmission();
     delay(10);
 }


 int getAHT(float *temperature, float *humidity)
 {
     int akn;
     uint32_t rawHumidity, rawTemperature;
 
     Wire.beginTransmission(AHT10_ADDR);
     Wire.write(0xAC);  // Messbefehl senden
     Wire.write(0x33);
     Wire.write(0x00);
     akn = Wire.endTransmission();
 
     delay(80);  // Wartezeit für Messung
 
     Wire.requestFrom(AHT10_ADDR, 6);
     if (Wire.available() < 6) {
         return -1;  // Fehler: Nicht genügend Daten
     }
 
     uint8_t data[6];
     for (int i = 0; i < 6; i++) {
         data[i] = Wire.read();
     }
 
     rawHumidity = ((uint32_t)data[1] << 16 | (uint32_t)data[2] << 8 | (uint32_t)data[3]) >> 4;
     rawTemperature = ((uint32_t)(data[3] & 0x0F) << 16 | (uint32_t)data[4] << 8 | (uint32_t)data[5]);
 
     *humidity = (rawHumidity * 100.0) / 1048576.0;
     *temperature = (rawTemperature * 200.0) / 1048576.0 - 50;
 
     return akn;
 }
 
 bool checkAHTConnection(void)
 {
     Wire.beginTransmission(AHT10_ADDR);
     if (Wire.endTransmission() == 0) {
         Serial.println("AHT10 Sensor gefunden.");
         return true;
     } else {
         Serial.println("AHT10 Sensor nicht gefunden!");
         return false;
     }
 }