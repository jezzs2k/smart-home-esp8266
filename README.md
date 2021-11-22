### SmartHome with esp8266 module

## Setup Evironment
- Open Arduno and go to Preferences
- Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs
- Install ESP8266 from Tools/Boards Manager and then choose NODE_MCU 1.0
- Install WifiManger: https://github.com/tzapu/WiFiManager in to LIB + Add lib by .zip file
- Install Firebase-ESP-Client : https://github.com/mobizt/Firebase-ESP-Client + Add lib by .zip file 
- Copy folder SD to libraries of the ardunos in the documents folders

## Generate QR CODE
- URL: https://vi.qr-code-generator.com/
- Gen ID URL: https://www.uuidgenerator.net/
- Text add inside the code QR: {"ssid":"SMART_HOME_ESP8266","password":"11111111","idEsp":"36d57abd-7e84-4079-afc0-cc9693a6dd90"}

## Libraries
```
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
```

## Global Value
```
bool isConnectWifi = false;
String presubDataFirebase = "/esp/";
String userIdGlobalValue = "";
String codeGlobalValue = "";
```

## Firebase Config

```
Api_key = "AIzaSyA4MflFmphgSw0Ae8mKwkh8_i6np4n8iK8";
EmailFirebaseAdmin = "hieu.opentechiz@gmail.com";
PasswordFirebaseAdmin = "Vuthanhhieu2000+";
Database_url = "https://smart-home-87480-default-rtdb.firebaseio.com/";
TokenFirebase = "QiKnkj3hlR4qaUfjvn631IeCEwPrXQFAhuMUihnV";
```

## HTML file to setup connect wifi
