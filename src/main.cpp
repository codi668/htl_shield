/******************************************************************
                        Simple State Machine
                                                    қuran nov 2022
******************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "init.h"
#include "adc.h"
#include "leds.h"
#include "oled.h"
#include "mfc.h"
#include "timer.h"
#include "aht.h"

#define STATE_0        0    // start
#define STATE_1        1    // poti
#define STATE_2        2    // drehregler
#define STATE_3        3    // blink
#define STATE_4        4    // adc
#define STATE_5        5    // mfc^
#define STATE_6        6    // peakhold
#define STATE_7        7    // aht

char text[TXT_LENGTH];
char tempStr[10];  // Puffer für die Temperatur als String
char humStr[10];   // Puffer für die Luftfeuchtigkeit als String


void showStartMessage(void);


void setup()
{
    Wire.begin();
    Serial.begin(115200);
    
    initHtlShield();
    initTimer(); 
    initOled();
    initMFC();

    Serial.println("start!");

}


void loop()
{
    static int state = STATE_0;
    static int led = FALSE; 
    int x, y, z;

    if (keyPressed(DIAL_KEY))    // für alle states: reset dialControl
    {
        clearKey(DIAL_KEY); // flagd = FALSE;
        resetDialControl();      // = 0; 
    }

    switch (state)
    {
        case STATE_0:

            if (keyPressed(KEY_1)) 
            {
                Serial.println("key 1 pressed!");
                state = STATE_1;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                oledClrDisplay();
                sprintf(text, "Poti:");
                oledPrintfxy(0, 0, text);
                oledRefresh();
                setMulticolorLed(0, 10, 10);
            }
        
        break; 

        case STATE_1:

            x = getPotiLin();
            setLeds(x); 

            x = getPoti();

            oledClrDisplay();
            sprintf(text, "Poti: %03d", x);
            oledPrintfxy(0, 0, text);
            oledRefresh();

            if (keyPressed(KEY_1))
            {
                state = STATE_2;
                clearKey(KEY_1); 
                clearKey(KEY_2); 
                setMulticolorLed(0, 0, 30);
            }

            if (keyPressed(KEY_2))
            {
                Serial.println("key 2 pressed!");
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                showStartMessage();
                setMulticolorLed(40, 0, 0);
            }

        break;
 
 
        case STATE_2:   // dial Control

            x = getDialControlCenter();
            setLeds(x);
            x = getDialControl();

            oledClrDisplay();
            sprintf(text, "Drehregler");
            oledPrintfxy(0,  0, text);
            sprintf(text, "d = %03d", x);
            oledPrintfxy(0, 20, text);
            oledRefresh();

            if (keyPressed(KEY_1))
            {
                state = STATE_3;
                clearKey(KEY_1); 
                clearKey(KEY_2); 
                startTimer(TIMER_0, ONE_SEC);                
                startTimer(TIMER_1, FIVE_SEC);                
                setMulticolorLed(40, 0, 0); 
            }


            if (keyPressed(KEY_2))
            {
                Serial.println("key 2 pressed!");
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                showStartMessage();
                setMulticolorLed(40, 0, 0);
            }

        break;

        case STATE_3:  // blink and jump after 5 sec to STATE_4

            oledClrDisplay();
            sprintf(text, "blink");
            oledPrintfxy(0, 0, text);
            oledRefresh();

            if (timerexpired(TIMER_0))
            {
                led = (led == TRUE) ? FALSE : TRUE;

                if (led == TRUE) setMulticolorLed(40, 0, 0); 
                else setMulticolorLed(0, 0, 0);

                startTimer(TIMER_0, ONE_SEC);
            }

            if (timerexpired(TIMER_1))
            {
                state = STATE_4;
            }
            
            if (keyPressed(KEY_1))
            {
                state = STATE_4;
                clearKey(KEY_1); 
                clearKey(KEY_2); 
                setMulticolorLed(0, 30, 0); 
            }

            if (keyPressed(KEY_2))
            {
                Serial.println("key 2 pressed!");
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                showStartMessage();
                setMulticolorLed(40, 0, 0);
            }

        break; 

        case STATE_4:  // adc

            x = getLDR();
            oledClrDisplay();
            sprintf(text, "ldr: %03d", x);
            oledPrintfxy(0, 0, text);
            oledRefresh();
            setMulticolorLed(40, 0, 0);

            if (keyPressed(KEY_1))
            {
                state = STATE_5;
                clearKey(KEY_1); 
                clearKey(KEY_2); 
                setMulticolorLed(40, 0, 0); 
            }

            if (keyPressed(KEY_2))
            {
                Serial.println("key 2 pressed!");
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                showStartMessage();
                setMulticolorLed(40, 0, 0);
            }
        break; 

        case STATE_5:  // mfc

            getMFC(&x, &y, &z);

            oledClrDisplay();
            sprintf(text, "xy = %03d", x);  oledPrintfxy(0,  0, text);
            sprintf(text, "yz = %03d", y);  oledPrintfxy(0, 20, text);
            sprintf(text, "zx = %03d", z);  oledPrintfxy(0, 40, text);
            oledRefresh();
            setMulticolorLed(40, 0, 0);


            if (keyPressed(KEY_1))
            {
                state = STATE_6;
                getPoti(); 
                clearKey(KEY_1); 
                clearKey(KEY_2); 
                setMulticolorLed(0, 20, 30);
            }


            if (keyPressed(KEY_2))
            {
                Serial.println("key 2 pressed!");
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                showStartMessage();
                setMulticolorLed(40, 0, 0);
            }
        break; 

        case STATE_6:  // peak hold
        
            oledClrDisplay();
            sprintf(text, "Peak x = %1d", getxPeak()); 
            oledPrintfxy(0, 0, text);
            oledRefresh();
            setMulticolorLed(40, 0, 0);
            
            Wire.beginTransmission(PCF8574);
            Wire.write(~getPeakLeds()); 

            if (keyPressed(KEY_1))
            {
                state = STATE_7;
                clearKey(KEY_1); 
                clearKey(KEY_2); 
                setMulticolorLed(40, 0, 0);
            }
        
            if (keyPressed(KEY_2))
            {
                Serial.println("key 2 pressed!");
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                showStartMessage();
                setMulticolorLed(40, 0, 0);
            }
        


        break;

        case STATE_7:  // aht

            getAHT(tempStr, humStr);

            oledClrDisplay();
            //oledPrintfxy(0,  0, tempStr);
            //oledPrintfxy(0, 20, humStr);
            oledRefresh();
            setMulticolorLed(40, 0, 0);

            if (keyPressed(KEY_1))
            {
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 
                setMulticolorLed(40, 0, 0);
                showStartMessage();
            }
            if (keyPressed(KEY_2))
            {
                Serial.println("key 2 pressed!");
                state = STATE_0;
                clearKey(KEY_1); 
                clearKey(KEY_2); 

                showStartMessage();
                setMulticolorLed(40, 0, 0);
            }
        break;


    }


}

void showStartMessage(void)
{
    oledClrDisplay();
    sprintf(text, "press T1");
    oledPrintfxy(0, 0, text);
    oledRefresh();
}