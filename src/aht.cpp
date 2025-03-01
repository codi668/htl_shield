#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <AHT10.h>
#include <Adafruit_SSD1306.h>
#include "aht.h"
#include "init.h"
#include "leds.h"
#include "oled.h"
#include "timer.h"

void initAHT(void)
{
    AHT10 aht; //AHT10 sensor
    if (!aht.begin())
    {
        Serial.println("Could not find AHTX0 sensor!");
        while (1)
            ;
    }
}
int getAHT(char *tempStr, char *humStr)
{
    float temperature, humidity;
    AHT10 aht;
    temperature = aht.readTemperature();
    
    humidity = aht.readHumidity();
    sprintf(tempStr, "Temp: %2.2f", temperature);
    sprintf(humStr, "Hum: %2.2f", humidity);

}
void floatToString(float value, char* buffer, int precision) {
    int integerPart = (int)value;
    float fractionalPart = value - integerPart;
    int fractionalInt = (int)(fractionalPart * pow(10, precision));

    sprintf(buffer, "%d.%0*d", integerPart, precision, fractionalInt);
}