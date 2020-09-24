#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <STM32SD.h>
#include <SPI.h>
#include "Ucglib.h"
#include "rtc.h"
#include <STM32RTC.h>

#define BLINK_LED LED_BUILTIN

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Ucglib_ST7735_18x128x160_SWSPI ucg(/*sclk=*/ PB13, /*data=*/ PB15, /*cd=*/ PB1, /*cs=*/ PB12, /*reset=*/ PD10);
Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ PB1, /*cs=*/ PB12, /*reset=*/ PD10);


#ifndef SD_DETECT_PIN
#define SD_DETECT_PIN SD_DETECT_NONE
#endif


/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 16;

/* Change these values to set the current initial date */
/* Monday 15th June 2015 */
const byte weekDay = 1;
const byte day = 15;
const byte month = 6;
const byte year = 15;

void print2digits(int number, Adafruit_SSD1306 * stream);
void printRtc(Adafruit_SSD1306 * stream);
/*
 [+] I2C1
 [+] DAC
 [+] ADC
 [+] UART1, UART2, UART3
 [+] SDIO
 [+] SPI
 [+] PWM
 [+] RTC
 [+] Tone
 [ ] SerialUSB
*/

void setup() 
{
    Wire.begin();
    SerialUSB.begin(9600);
    Serial.begin(9600);
    Serial2.begin(9600);
    Serial3.begin(9600);

    // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
    // By default the LSI is selected as source.
    //rtc.setClockSource(STM32RTC::LSE_CLOCK);

    rtc.begin(); // initialize RTC 24H format

    // Set the time
    rtc.setHours(hours);
    rtc.setMinutes(minutes);
    rtc.setSeconds(seconds);

    // Set the date
    rtc.setWeekDay(weekDay);
    rtc.setDay(day);
    rtc.setMonth(month);
    rtc.setYear(year);

    pinMode(BLINK_LED, OUTPUT);
    pinMode(PB0, OUTPUT);

    digitalWrite(PB0, HIGH);

    Serial.println(millis());

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
    { 
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    display.display();
    display.clearDisplay();
    display.display();

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Draw white text
    display.cp437(true);         // Use full 256 char 'Code Page 437' font

    display.setCursor(0, 0);

    if (SD.begin(SD_DETECT_PIN))
    {
        display.print("SD OK");
    } 
    else 
    {
        display.print("NO SD");
    }

    display.display();

    ucg.begin(UCG_FONT_MODE_TRANSPARENT);
    ucg.clearScreen();

    tone(PE5, 2000);
    analogWrite(PE4, 10);
}

void dac1()
{
    for(int i = 0; i < 250; i++)
    {
        analogWrite(PA4, i);
        analogWrite(PA5, 255 - i);
        delay(2);
    }
}

void loop() 
{
    ucg.setFont(ucg_font_ncenR12_tr);
    ucg.setColor(0, 255, 0, 0);
    
    ucg.setPrintPos(0,25);
    ucg.print(millis());

    digitalWrite(BLINK_LED, HIGH);
    dac1();
    digitalWrite(BLINK_LED, LOW);
    delay(500);

    for (int i = 0; i < NUM_ANALOG_INPUTS - 2; i++)
    {
        int val = analogRead(i);
        Serial.print(i); Serial.print(" = "); Serial.println(val);
    }

    display.setCursor(0, 9);
    display.println(millis());
    display.setCursor(0, 17);
    printRtc(&display);
    display.display();

    Serial.println(millis());
    Serial2.println(millis());
    Serial3.println(millis());
    SerialUSB.println(millis());

    while (Serial.available()) Serial.write(Serial.read());
    while (Serial2.available()) Serial2.write(Serial2.read());
    while (Serial3.available()) Serial3.write(Serial3.read());
}

void printRtc(Adafruit_SSD1306 * stream)
{
      // Print date...
  print2digits(rtc.getDay(), stream);
  stream->print("/");
  print2digits(rtc.getMonth(), stream);
  stream->print("/");
  print2digits(rtc.getYear(), stream);
  stream->print(" ");

  // ...and time
  print2digits(rtc.getHours(), stream);
  stream->print(":");
  print2digits(rtc.getMinutes(), stream);
  stream->print(":");
  print2digits(rtc.getSeconds(), stream);
}

void print2digits(int number, Adafruit_SSD1306 * stream) 
{
  if (number < 10) 
  {
    stream->print("0"); // print a 0 before if the number is < than 10
  }
  stream->print(number);
}
