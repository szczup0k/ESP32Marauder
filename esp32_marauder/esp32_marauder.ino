/* FLASH SETTINGS
Board: LOLIN D32
Flash Frequency: 80MHz
Partition Scheme: Minimal SPIFFS
https://www.online-utility.org/image/convert/to/XBM
*/

#include <WiFi.h>
#include <Wire.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <Arduino.h>
#include <Preferences.h>


#include "Assets.h"
#include "Display.h"
#include "WiFiScan.h"
#include "MenuFunctions.h"
#include "SDInterface.h"
#include "Web.h"
#include "Buffer.h"
#include "BatteryInterface.h"
//#include "icons.h"

Display display_obj;
WiFiScan wifi_scan_obj;
MenuFunctions menu_function_obj;
SDInterface sd_obj;
Web web_obj;
Buffer buffer_obj;
BatteryInterface battery_obj;

Preferences preferences;

uint32_t currentTime  = 0;

void setup()
{
  Serial.begin(115200);

  Serial.println("\n\n-------------------------------------\n");

  pinMode(FLASH_BUTTON, INPUT);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);

  preferences.begin("my-app", false);

  unsigned int counter = preferences.getUInt("counter", 0);

  if (counter == 0) {
    counter++;
    // Print the counter to Serial Monitor
    Serial.printf("Current counter value: %u\n", counter);
  
    // Store the counter to the Preferences
    preferences.putUInt("counter", counter);
  
    // Close the Preferences
    preferences.end();

    Serial.println("Initial reboot...");
    
    ESP.restart();
  }
  else {
    Serial.println("Initial reboot complete");
    counter = 0;
    // Print the counter to Serial Monitor
    Serial.printf("Current counter value: %u\n", counter);
  
    // Store the counter to the Preferences
    preferences.putUInt("counter", counter);
  
    // Close the Preferences
    preferences.end();
  }

  // Preset SPI CS pins to avoid bus conflicts
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SD_CS, HIGH);
  
  Serial.println("\n\n--------------------------------\n");
  Serial.println("         ESP32 Marauder      \n");
  Serial.println("            " + display_obj.version_number + "\n");
  Serial.println("       By: justcallmekoko\n");
  Serial.println("--------------------------------\n\n");

  // Do some SD stuff
  if(sd_obj.initSD())
    Serial.println("SD Card supported");
  else
    Serial.println("SD Card NOT Supported");

  // Run display setup
  display_obj.RunSetup();

  // Build menus
  menu_function_obj.RunSetup();

  battery_obj.RunSetup();

  battery_obj.battery_level = battery_obj.getBatteryLevel();

  if (battery_obj.i2c_supported) {
    Serial.println("IP5306 I2C Supported: true");
  }
  else
    Serial.println("IP5306 I2C Supported: false");
}


void loop()
{
  
  // get the current time
  //if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM))
  currentTime = millis();

  // Update all of our objects
  //if ((!display_obj.draw_tft) &&
  //    (wifi_scan_obj.currentScanMode != OTA_UPDATE))
  if (!display_obj.draw_tft)
  {
    display_obj.main(); 
    wifi_scan_obj.main(currentTime);
    sd_obj.main(currentTime);
    //if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM))
    if ((wifi_scan_obj.currentScanMode != WIFI_PACKET_MONITOR) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_EAPOL))
      menu_function_obj.main();
      if (wifi_scan_obj.currentScanMode == OTA_UPDATE)
        web_obj.main();
    delay(1);
  }
  else if ((display_obj.draw_tft) &&
           (wifi_scan_obj.currentScanMode != OTA_UPDATE))
  {
    display_obj.drawStylus();
  }
  //else
  //{
  //  web_obj.main();
  //}

  //Serial.println(wifi_scan_obj.currentScanMode);

  //Serial.print("Run Time: ");
  //Serial.print(millis() - currentTime);
  //Serial.println("ms");
}
