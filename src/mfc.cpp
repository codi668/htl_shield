/******************************************************************
                        all inits
                                                    қuran nov 2022
******************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "init.h"
#include "oled.h"
#include "mfc.h"

int Xmin = 0, Xmax = 0, Ymin = 0, Ymax = 0, Zmin = 0, Zmax = 0;

char text_random[TXT_LENGTH];

void initMFC(void)
{
    Wire.beginTransmission(MFS);
    Wire.write(0x02); // select mode register
    Wire.write(0x00); // continuous measurement mode
    Wire.endTransmission();
}
void calculateAngles(int x, int y, int z, int* angleXY, int* angleYZ, int* angleZX) {
    // Aktualisiere Min- und Max-Werte
    if (x > Xmax) Xmax = x; 
    if (y > Ymax) Ymax = y;
    if (z > Zmax) Zmax = z;

    if (x < Xmin) Xmin = x;
    if (y < Ymin) Ymin = y;
    if (z < Zmin) Zmin = z;

    // Berechne den Nullpunkt (Offset) für jede Achse
    int offsetX = (Xmin + Xmax) / 2; // Nullpunkt für X-Achse
    int offsetY = (Ymin + Ymax) / 2; // Nullpunkt für Y-Achse
    int offsetZ = (Zmin + Zmax) / 2; // Nullpunkt für Z-Achse

    // Berechne die Skalierung für jede Achse
    float scaleX = (Xmax - Xmin) / 2.0; // Skalierung für X-Achse
    float scaleY = (Ymax - Ymin) / 2.0; // Skalierung für Y-Achse
    float scaleZ = (Zmax - Zmin) / 2.0; // Skalierung für Z-Achse

    // Kalibrierte Werte berechnen
    float calibratedX = (x - offsetX) / scaleX;
    float calibratedY = (y - offsetY) / scaleY;
    float calibratedZ = (z - offsetZ) / scaleZ;

    // Winkelberechnung mit kalibrierten Werten
    *angleXY = atan2(calibratedY, calibratedX) * 180 / M_PI;  // Winkel in der XY-Ebene
    *angleYZ = atan2(-calibratedZ, -calibratedY) * 180 / M_PI;  // Winkel in der YZ-Ebene
    *angleZX = atan2(calibratedX, -calibratedZ) * 180 / M_PI;  // Winkel in der ZX-Ebene

    // Winkel in den Bereich [0°, 360°] bringen
    if (*angleXY < 0) *angleXY += 360;
    if (*angleYZ < 0) *angleYZ += 360;
    if (*angleZX < 0) *angleZX += 360;
}

int getMFC(int *x, int *y, int *z)
{
    int akn;
    int angleXY, angleYZ, angleZX;


    Wire.beginTransmission(MFS);
    Wire.write(0x03);
    akn = Wire.endTransmission();

    Wire.requestFrom(MFS, 6);
    if (6 <= Wire.available())
    {
        *x = Wire.read() << 8;
        *x |= Wire.read();
        *z = Wire.read() << 8;
        *z |= Wire.read();
        *y = Wire.read() << 8;
        *y |= Wire.read();

        // Kalibrierung
        calculateAngles(*x, *y, *z, &angleXY, &angleYZ, &angleZX);
    }

    *x = angleXY;
    *y = angleYZ;
    *z = angleZX;

    return akn;
}
