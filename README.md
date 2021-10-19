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
Api_key = "AIzaSyA4MflFmphgSw0Ae8mKwkh8_i6np4n8iK8";
EmailFirebaseAdmin = "hieu.opentechiz@gmail.com";
PasswordFirebaseAdmin = "Vuthanhhieu2000+";
Database_url = "https://smart-home-87480-default-rtdb.firebaseio.com/";
TokenFirebase = "QiKnkj3hlR4qaUfjvn631IeCEwPrXQFAhuMUihnV";
```

## HTML file to setup connect wifi
