/*
 * buerodemo.ino
 * Kleine IoT-Demo für meine Bürotür.
 * 
 * Hardware: ESP32 Dev Module, 4MB (1.2MB APP/1.5MB SPIFFS), 240MHz, QIO, 4MB (32Mb)
 *           1.3' I2C OLED Display
 *           Dallas 18B20 temp sensor
 */

#define DEBUG 1

#include <ss_oled.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PString.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "RestCall.h"
#include <IoTGuru.h>
#include "credentials.h"

// GPIO where the DS18B20 is connected to
const int oneWireBus = 32;                                  // wifi network password

WiFiClient  wifiClient;
RestCall restClient = RestCall("api.net-things.de", 80);

SSOLED ssoled;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

IoTGuru iotGuru = IoTGuru(userShortId, deviceShortId, deviceKey);

// memorize current case for subsequent loop calls
int choice = 0;
// scrolling offset
int xoffset = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");

  // init OLED display
  int rc;
  // put your setup code here, to run once:
  rc = oledInit(&ssoled, OLED_128x64, -1, 0, 0, 1, -1, -1, -1, 400000L); // use standard I2C bus at 400Khz
  if (rc != OLED_NOT_FOUND)
  {
    char *msgs[] = {"SSD1306 @ 0x3C", "SSD1306 @ 0x3D","SH1106 @ 0x3C","SH1106 @ 0x3D"};
    oledFill(&ssoled, 0, 1);
    oledWriteString(&ssoled, 0,0,0,msgs[rc], FONT_NORMAL, 0, 1);
    delay(2000);
  }

  // Start the DS18B20 sensor
  sensors.begin();

  // connect WiFi
  connectWiFi();
  
  // Init OTA (over-the-air-update)
  ArduinoOTA.setHostname("esp32-B852-512");
  ArduinoOTA.setPassword("");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  ArduinoOTA.begin();
  
  randomSeed(1);

  // Init IoT GURU
  iotGuru.setCheckDuration(300000);
  iotGuru.setDebugPrinter(&Serial);
}

void loop() {
  ArduinoOTA.handle();
  iotGuru.check();

  if (choice != 3 || xoffset == 0)
    displayHeader();
  char buffer[20];
  PString out(buffer, sizeof(buffer));
  float temperatureC = 0;
  switch (choice) {
    case 1: {
      oledWriteString(&ssoled, 0,0,4,(char *)"Aktuelle", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0,0,5,(char *)"Buerotemperatur:", FONT_NORMAL, 0, 1);
      sensors.requestTemperatures(); 
      temperatureC = sensors.getTempCByIndex(0);
      out.print(temperatureC); out.print(" Celsius");
      oledWriteString(&ssoled, 0,0,7, buffer, FONT_NORMAL, 0, 1);
      if (WiFi.status() != WL_CONNECTED) connectWiFi();
      
      // Write to IoT GURU
      iotGuru.sendHttpValue(nodeKey, fieldName, temperatureC);
      choice++;
      delay(4000); }
    break;
    
    case 2:
      oledWriteString(&ssoled, 0,0,4,(char *)"Ich bin gerade:", FONT_NORMAL, 0, 1);
      if (WiFi.status() != WL_CONNECTED) connectWiFi();
      if (restClient.get("/whereami") == 200) {
        String text = restClient.readNextLine();
        if (text.length() > 21) {
          oledWriteString(&ssoled, 0,0,6,(char *)text.substring(0,21).c_str(), FONT_SMALL, 0, 1);
          oledWriteString(&ssoled, 0,0,7,(char *)text.substring(21).c_str(), FONT_SMALL, 0, 1);
        }
        else
          oledWriteString(&ssoled, 0,0,6,(char *)text.c_str(), FONT_SMALL, 0, 1);
        
      }
      else {
        oledWriteString(&ssoled, 0,0,6,(char *)"(Sorry, es wurde kein", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled, 0,0,7,(char *)"aktueller Ort angegeben)", FONT_SMALL, 0, 1);
      }
      restClient.stop();
      choice++;
      delay(8000);
    break;

    case 3:
      oledWriteString(&ssoled, xoffset,0,4,(char *)"Hinterlasse auf demo.net-things.de Deine persoenliche Nachricht!", FONT_LARGE, 0, 1);
      xoffset += 8;
      if (xoffset > 1150) {
        choice++;
        xoffset = 0;
        delay(500);
      }
      break;
      
    case 4:
      oledWriteString(&ssoled, 0,0,4,(char *)"Letzte Usernachricht:", FONT_SMALL, 0, 1);
      if (WiFi.status() != WL_CONNECTED) connectWiFi();
      if (restClient.get("/usermessage") == 200) {
        String text = restClient.readNextLine();
        if (text.length() > 21) {
          oledWriteString(&ssoled, 0,0,6,(char *)text.substring(0,21).c_str(), FONT_SMALL, 0, 1);
          oledWriteString(&ssoled, 0,0,7,(char *)text.substring(21).c_str(), FONT_SMALL, 0, 1);
        }
        else
          oledWriteString(&ssoled, 0,0,6,(char *)text.c_str(), FONT_SMALL, 0, 1);
        
      }
      else {
        oledWriteString(&ssoled, 0,0,6,(char *)"(keine Nachricht", FONT_NORMAL, 0, 1);
        oledWriteString(&ssoled, 0,0,7,(char *)"hinterlassen)", FONT_NORMAL, 0, 1);
      }
      restClient.stop();
      choice++;
      delay(6000);
    break;

    default:
      oledWriteString(&ssoled, 0,0,4,(char *)"IoT Demo", FONT_LARGE, 0, 1);
      oledWriteString(&ssoled, 0,8,7,(char *)"Internet of Things", FONT_SMALL, 0, 1);
      choice = 1;
      delay(4000);
    break;
  }
  delay(100);
}

void displayHeader() {
  oledFill(&ssoled, 0, 1);
  delay(50);
  oledWriteString(&ssoled, 0,0,0,(char *)"Alexander Schmitt", FONT_SMALL, 0, 1);
  delay(20);
  oledWriteString(&ssoled, 0,0,1,(char *)"Digitalisierungs-", FONT_SMALL, 1, 1);
  oledWriteString(&ssoled, 0,0,2,(char *)"manager", FONT_SMALL, 1, 1);
  
}

void connectWiFi() {
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int tryout = 0;
  while (WiFi.status() != WL_CONNECTED && tryout++ < 6) {
    Serial.print(".");
    // wait 1.5 seconds for re-trying
    delay(1500);
  }
  if (WiFi.status() != WL_CONNECTED)  // reboot if WiFi cannot be connected
    ESP.restart();
      
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
