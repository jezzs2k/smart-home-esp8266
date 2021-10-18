### SmartHome with esp8266 module


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
Api_key = "7234a488911316bedff11c65c28f361954a97051";
EmailFirebaseAdmin = "vuthanhhieu00@gmail.com";
PasswordFirebaseAdmin = "vuthanhhieu2000";
Database_url = "https://htcdt-iot-default-rtdb.firebaseio.com/";
TokenFirebase = "KbjQ9IW5NEZhcikz8mA4B5LYHWWYQJo3wOYnUWyp";
```

## HTML file to setup connect wifi
