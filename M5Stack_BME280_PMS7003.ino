/*
  http://downloads.arduino.cc/packages/package_arduino.cc_linux_index.json
  https://github.com/espressif/arduino-esp32/releases/download/1.0.2-rc1/package_esp32_dev_index.json
  http://arduino.esp8266.com/stable/package_esp8266com_index.json
  https://dl.espressif.com/dl/package_esp32_index.json

 * PMS7003 - Serial
 * BME280 - I2C1
 * Dudals Jung
 * miscellany@gmail.com
 * www.dudals.net
 * 2019.03.14 
 */
 
#include <M5Stack.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include "BlueDot_BME280.h"

#define SEALEVELPRESSURE_HPA (1021.7)
#define PMS7003_RxMAX 32

#define IWIDTH  320
#define IHEIGHT 240

BlueDot_BME280 bme280;
int bme280_Detected = 0;

int BatteryLevel = 0;

int PMS7003_PM1_0 = 0;
int PMS7003_PM2_5 = 0;
int PMS7003_PM10 = 0;

float BME280_TempC = 0;
float BME280_Humidity = 0;
float BME280_Pressure = 0;
float BME280_AltitudeMeter = 0;

uint16_t PMS7003_PM1_0_RangeMax[4] = {15, 35,  75, 300}; // 0~15 Good, 16~35 Normal, 36~75 Bad, 75~ Very bad
uint16_t PMS7003_PM2_5_RangeMax[4] = {15, 35,  75, 300};
uint16_t PMS7003_PM10_RangeMax[4] =  {30, 80, 100, 300};

TFT_eSprite img = TFT_eSprite(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object

// Testing 
int8_t GetBatteryLevel()
{
    Wire.beginTransmission(0x75);
    Wire.write(0x78);
    
    BatteryLevel = -1;

    if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1)) 
    {
        switch (Wire.read() & 0xF0) 
        {
            case 0xE0: 
                BatteryLevel = 25;
            case 0xC0: 
                BatteryLevel = 50;
            case 0x80: 
                BatteryLevel = 75;
            case 0x00: 
                BatteryLevel = 100;
            default: 
                BatteryLevel = 0;
        }
    }
}

void task_active(void * pvParameters) 
{
    char textBuffer[50];
    int TextColor = 0;
    
    for(;;) 
    {
        img.fillRect(0, 50, 320, 190, TFT_BLACK);

        img.setTextDatum(TC_DATUM);

        img.setFreeFont(FM9);

        sprintf(textBuffer,"Battery Level : %3d ", BatteryLevel);
        img.drawString(textBuffer, 160, 20, GFXFF);// Print the string name of the font

        img.setTextDatum(TL_DATUM);

        img.setFreeFont(FM18);

        if (PMS7003_PM1_0 <= PMS7003_PM1_0_RangeMax[0])
            TextColor = TFT_BLUE;
        else if (PMS7003_PM1_0 <= PMS7003_PM1_0_RangeMax[1])
            TextColor = TFT_GREEN;
        else if (PMS7003_PM1_0 <= PMS7003_PM1_0_RangeMax[2])
            TextColor = TFT_YELLOW;
        else
            TextColor = TFT_RED;
        
        sprintf(textBuffer,"PM 1.0 : %3d\n", PMS7003_PM1_0);
        img.drawString(textBuffer, 10, 50, GFXFF);// Print the string name of the font

        img.fillCircle(290, 50 + 10 + 2, 10, TextColor); // now

        img.fillRect(10, 80, PMS7003_PM1_0_RangeMax[0], 3, TFT_BLUE); // 0 ~ 15 Good
        img.fillRect(10 + PMS7003_PM1_0_RangeMax[0], 80, PMS7003_PM1_0_RangeMax[1] - PMS7003_PM1_0_RangeMax[0], 3, TFT_GREEN); // 16 ~ 35 Normal
        img.fillRect(10 + PMS7003_PM1_0_RangeMax[1], 80, PMS7003_PM1_0_RangeMax[2] - PMS7003_PM1_0_RangeMax[1], 3, TFT_YELLOW); // 36 ~ 75 Bad
        img.fillRect(10 + PMS7003_PM1_0_RangeMax[2], 80, PMS7003_PM1_0_RangeMax[3] - PMS7003_PM1_0_RangeMax[2], 3, TFT_RED); // 76 ~ Very Bad

        img.fillTriangle(10 + PMS7003_PM1_0 - 3, 95, 10 + PMS7003_PM1_0, 82,10 + PMS7003_PM1_0 + 3, 95, TFT_WHITE);

        if (PMS7003_PM2_5 <= PMS7003_PM2_5_RangeMax[0])
            TextColor = TFT_BLUE;
        else if (PMS7003_PM2_5 <= PMS7003_PM2_5_RangeMax[1])
            TextColor = TFT_GREEN;
        else if (PMS7003_PM2_5 <= PMS7003_PM2_5_RangeMax[2])
            TextColor = TFT_YELLOW;
        else
            TextColor = TFT_RED;
        
        sprintf(textBuffer,"PM 2.5 : %3d\n", PMS7003_PM2_5);
        img.drawString(textBuffer, 10, 100, GFXFF);// Print the string name of the font

        img.fillCircle(290, 100 + 10 + 2, 10, TextColor); // now

        img.fillRect(10, 130, PMS7003_PM2_5_RangeMax[0], 3, TFT_BLUE); // 0 ~ 15 Good
        img.fillRect(10 + PMS7003_PM2_5_RangeMax[0], 130, PMS7003_PM2_5_RangeMax[1] - PMS7003_PM2_5_RangeMax[0], 3, TFT_GREEN); // 16 ~ 35 Normal
        img.fillRect(10 + PMS7003_PM2_5_RangeMax[1], 130, PMS7003_PM2_5_RangeMax[2] - PMS7003_PM2_5_RangeMax[1], 3, TFT_YELLOW); // 36 ~ 75 Bad
        img.fillRect(10 + PMS7003_PM2_5_RangeMax[2], 130, PMS7003_PM2_5_RangeMax[3] - PMS7003_PM2_5_RangeMax[2], 3, TFT_RED); // 76 ~ Very Bad

        img.fillTriangle(10+PMS7003_PM2_5 - 3, 145, 10 + PMS7003_PM2_5, 132,10 + PMS7003_PM2_5 + 3, 145, TFT_WHITE);

        if (PMS7003_PM10 <= PMS7003_PM10_RangeMax[0])
            TextColor = TFT_BLUE;
        else if (PMS7003_PM10 <= PMS7003_PM10_RangeMax[1])
            TextColor = TFT_GREEN;
        else if (PMS7003_PM10 <= PMS7003_PM10_RangeMax[2])
            TextColor = TFT_YELLOW;
        else
            TextColor = TFT_RED;
        
        sprintf(textBuffer,"PM 10  : %3d\n", PMS7003_PM10);
        img.drawString(textBuffer, 10, 150, GFXFF);// Print the string name of the font

        img.fillCircle(290, 150 + 10 + 2, 10, TextColor); // now

        img.fillRect(10, 180, PMS7003_PM10_RangeMax[0], 3, TFT_BLUE); // 0 ~ 30 Good
        img.fillRect(10 + PMS7003_PM10_RangeMax[0], 180, PMS7003_PM10_RangeMax[1] - PMS7003_PM10_RangeMax[0], 3, TFT_GREEN); // 31 ~ 80 Normal
        img.fillRect(10 + PMS7003_PM10_RangeMax[1], 180, PMS7003_PM10_RangeMax[2] - PMS7003_PM10_RangeMax[1], 3, TFT_YELLOW); // 81 ~ 100 Bad
        img.fillRect(10 + PMS7003_PM10_RangeMax[2], 180, PMS7003_PM10_RangeMax[3] - PMS7003_PM10_RangeMax[2], 3, TFT_RED); // 100 ~ Very Bad

        img.fillTriangle(10 + PMS7003_PM10 - 3, 195, 10 + PMS7003_PM10, 182,10 + PMS7003_PM10 + 3, 195, TFT_WHITE);

        img.setTextDatum(TC_DATUM);
        img.setFreeFont(FM12);

        img.setTextColor(TFT_WHITE);
        sprintf(textBuffer,"%2.1f'C %2.1f%% %4.2fhPa", BME280_TempC, BME280_Humidity, BME280_Pressure);
        img.drawString(textBuffer, 160, 218, GFXFF);// Print the string name of the font

        img.pushSprite(0, 0);
 
        delay(500);
    }
}

void task_pms7003(void * pvParameters) 
{
    uint8_t pms7003_rx[PMS7003_RxMAX]; 
//    uint8_t pms7003_rx_index = 0;
    char RxChar;

    for(;;) 
    {
        while (Serial.available()) 
        {
            char RxChar = (char) Serial.read();

            pms7003_rx[PMS7003_RxMAX-1] = RxChar; 
            memmove(pms7003_rx, pms7003_rx+1, PMS7003_RxMAX-1);              

            if ((pms7003_rx[0] == 0x42) && (pms7003_rx[1] == 0x4d))
            {
                // Checksum 체크 필요
                
                PMS7003_PM1_0 = (pms7003_rx[10] <<8) | pms7003_rx[11];
                PMS7003_PM2_5 = (pms7003_rx[12] <<8) | pms7003_rx[13];
                PMS7003_PM10  = (pms7003_rx[14] <<8) | pms7003_rx[15];
            }
        }
        delay(1);
    }
}

void task_bme280(void * pvParameters) 
{
    for(;;) 
    {
        if (bme280_Detected)
        {
            BME280_TempC = bme280.readTempC();
            BME280_Humidity = bme280.readHumidity();
            BME280_Pressure = bme280.readPressure();
            BME280_AltitudeMeter = bme280.readAltitudeMeter();
        }

        GetBatteryLevel();
       
        delay(500);
    }
}

void setup() 
{
    M5.begin();
    Wire.begin();
  
    M5.Lcd.fillScreen(TFT_BLACK);
    
    img.setColorDepth(8);
    img.createSprite(IWIDTH, IHEIGHT);
    img.fillSprite(TFT_BLACK);    
    
    Serial.begin(9600,SERIAL_8N1);

    img.print("        M5Stack / PMS7003(serial) / BME280(I2C)\n");
    img.print("  Dudals Jung (miscellany@gmail.com) www.dudals.net\n");

    bme280.parameter.communication = 0;
    bme280.parameter.I2CAddress = 0x76; // SDO = GND
    bme280.parameter.sensorMode = 0b11;
    bme280.parameter.IIRfilter = 0b100;
    bme280.parameter.humidOversampling = 0b101;
    bme280.parameter.tempOversampling = 0b101;
    bme280.parameter.pressOversampling = 0b101;
    bme280.parameter.pressureSeaLevel = 1013.25;
    bme280.parameter.tempOutsideCelsius = 15;
    bme280.parameter.tempOutsideFahrenheit = 59;

    if (bme280.init() != 0x60)
    {    
        img.print("Ops! First BME280 Sensor not found!\n");
        bme280_Detected = 0;
    }
    else
    {
        bme280_Detected = 1;
    }

    xTaskCreatePinnedToCore(task_pms7003, "task_pms7003", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(task_bme280,  "task_bme280",  4096, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(task_active,  "task_active",  4096, NULL, 3, NULL, 0);
}

void loop() 
{
    M5.update();
}
