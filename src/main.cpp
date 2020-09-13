#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BLINK_LED LED_BUILTIN

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() 
{
    Wire.begin();
    Serial.begin(9600);
    pinMode(BLINK_LED, OUTPUT);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    display.display();
    display.clearDisplay();
    display.display();
}

void testdrawchar(void) 
{
    display.clearDisplay();

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font

    // Not all the characters will fit on the display. This is normal.
    // Library will draw what it can and the rest will be clipped.
    for(int16_t i=0; i<256; i++) 
    {
        if(i == '\n') display.write(' ');
        else          display.write(i);
    }

    display.display();
    delay(2000);
}

void i2c_scan()
{
    byte error, address;
    int nDevices;

    nDevices = 0;
    for(address = 1; address < 127; address++ )
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            //Serial.print("I2C device found at address 0x");
            // if (address<16)
            // Serial.print("0");
            //Serial.print(address,HEX);
            // Serial.println("  !");

            nDevices++;
        }
        else if (error==4)
        {
            //Serial.print("Unknown error at address 0x");
            // if (address<16)
            //     Serial.print("0");
             Serial.println(address,HEX);
        }    
    }
    // if (nDevices == 0)
    //     Serial.println("No I2C devices found\n");
    // else
    //     Serial.println("done\n");

    delay(1000);           // wait 5 seconds for next scan
}

void loop() 
{
    testdrawchar();

    digitalWrite(BLINK_LED, HIGH);
    delay(500);
    digitalWrite(BLINK_LED, LOW);
    delay(500);
    for (int i = 0; i < NUM_ANALOG_INPUTS; i++)
    {
        int val = analogRead(i);
        //Serial.print(i); Serial.print(" = "); Serial.println(val/1000, 3);
    }

    //i2c_scan();
}