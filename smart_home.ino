#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>

/*HTML Defind*/
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Smart Home + ESP8266 kết nối WiFi bằng điện thoại</title>
    <style>
        body {
            font-family: "Open Sans", sans-serif;
            height: 100vh;
        }
        * {
            box-sizing: border-box;
        }
        @keyframes spinner {
            0% { transform: rotateZ(0deg); }
            100% { transform: rotateZ(359deg); }
        }
        div.wrapper {
            display: flex;
            align-items: center;
            flex-direction: column; 
            justify-content: center;
            width: 100%;
            min-height: 100%;
            padding: 20px;
            background: rgba(darken("#2196F3",40%), 0.85);
        }
        .login {
            border-radius: 2px 2px 5px 5px;
            padding: 10px 20px 20px 20px;
            width: 90%;
            max-width: 320px;
            background: #ffffff;
            position: relative;
            padding-bottom: 80px;
            box-shadow: 0px 1px 5px rgba(0,0,0,0.3);
        }

        input {
            display: block;
            padding: 15px 10px;
            margin-bottom: 10px;
            width: 100%;
            border: 1px solid #ddd;
            transition: border-width 0.2s ease;
            border-radius: 2px;
            color: #ccc;  
        }

        a {
            font-size: 0.8em;   
            color: #2196F3;
            text-decoration: none;
        }

        .title {
            color: #444;
            font-size: 1.2em;
            font-weight: bold;
            margin: 10px 0 30px 0;
            border-bottom: 1px solid #eee;
            padding-bottom: 20px;
        }

        input.state {
            width: 100%;
            height: 100%;
            padding: 10px 10px;
            background: #2196F3;
            color: #fff;
            display: block;
            border: none;
            margin-top: 20px;
            position: absolute;
            left: 0;
            bottom: 0;
            max-height: 60px;
            border: 0px solid rgba(0,0,0,0.1);
            border-radius: 0 0 2px 2px;
            transform: rotateZ(0deg);
            transition: all 0.1s ease-out;
            border-bottom-width: 7px;
        }

        .spinner {
            display: block;
            width: 40px;
            height: 40px;
            position: absolute;
            border: 4px solid #ffffff;
            border-top-color: rgba(255,255,255,0.3);
            border-radius: 100%;
            left: 50%;
            top: 0;
            opacity: 0;
            margin-left: -20px;
            margin-top: -20px;
            animation: spinner 0.6s infinite linear;
            transition: top 0.3s 0.3s ease,
                        opacity 0.3s 0.3s ease,
                        border-radius 0.3s ease;
            box-shadow: 0px 1px 0px rgba(0,0,0,0.2);
        }

        footer {
            display: block;
            padding-top: 50px;
            text-align: center;
            color: #ddd;
            font-weight: normal;
            text-shadow: 0px -1px 0px rgba(0,0,0,0.2);
            font-size: 0.8em;
        }
            
    </style>
</head>
<body>

    <div class="wrapper">
        <form class="login" action="setup">
          <p class="title">Đăng nhập wifi cho module này</p>
          <input type="text" placeholder="Tên wifi nhà bạn" autofocus name="ssid"/>
          <input type="password" placeholder="Mật khẩu wifi nhà bạn" name="password" />
          <input class="state" type="submit" value="Xác nhận"/>
        </form>
        <footer><a target="blank" href="http://google.com/">Smart Home IOT</a></footer>
        </p>
      </div>

</body>
</html>
)=====";
/*------------------------------------------------------*/

/*DEFAULT_VALUE*/
String idThisEsp = "62200f8c-35be-11ec-8d3d-0242ac130003";
const char *ssidLocal = "SMART_HOME_ESP8266";
const char *passwordLocal = "11111111";
int maxSizeEeprom = 160;
/*------------------------------------------------------*/

/*Global value*/
bool isConnectWifi = false;
String presubDataFirebase = "/esp/";
String userIdGlobalValue = "";
String turnOnValue = "";
String isResetEepromUserId = "";
String isResetEeprom = "";
bool isCheckExistEsp = false;
/*------------------------------------------------------*/

/*Firebase CONFIG*/
#define PROJECT_ID "smart-home-87480"

const char *api_key = "AIzaSyA4MflFmphgSw0Ae8mKwkh8_i6np4n8iK8";
const char *emailFirebaseAdmin = "hieu.opentechiz@gmail.com";
const char *passwordFirebaseAdmin = "Vuthanhhieu2000+";
const char *database_url = "https://smart-home-87480-default-rtdb.firebaseio.com/";
const char *tokenFirebase = "QiKnkj3hlR4qaUfjvn631IeCEwPrXQFAhuMUihnV";
/*------------------------------------------------------

/* Firebase defind */
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
/*------------------------------------------------------*/

/*Firebase ref url*/
String urlRequestGetUser = "/" + idThisEsp + "/isActive";
String urlRequestSetNewItem = "/" + idThisEsp + "/isNewItem";
String urlRequestGetUserError = "/" + idThisEsp + "/isError";
String urlRequestSetUser = "/" + idThisEsp + "/setUser";
String urlRequestConnectedUser = "/" + idThisEsp + "/isConnected";
String urlRequestResetUserIdEeprom = "/" + idThisEsp + "/isResetUserIdEeprom";
String urlRequestResetEeprom = "/" + idThisEsp + "/isResetEeprom";
String urlRequestTurnOnOff = "/" + idThisEsp + "/isTurnOn";
String urlCheckEspExistOnFirebase = "/" + idThisEsp;
/*------------------------------------------------------*/


ESP8266WebServer server(80); // Start server with port 80
WiFiManager wifiManager;

void connectIpEsp(){
  server.send(200,"text/html",MAIN_page); // Send HTML
}

void connectIpEspWhenConnectSuccess(){
  server.send(200,"text/html", "Helloword"); // Send HTML
}

void cleanEEProm (int from, int to, bool isResetEsp = true){
  Serial.println("Start delete eeprom: ");

  for (int i = from; i < to; ++i) {
     EEPROM.write(i, 0);
  };
 
  EEPROM.commit();
  wifiManager.resetSettings();

  Serial.println("Delete data eeprom success");

  if(isResetEsp) {
    ESP.reset(); 
  }
};

void addDataToEeprom (int pos, String name, String data) {
   Serial.print("Write data ");
   Serial.print(name);
   Serial.println(" eeprom.");

   if(data.length() <= 0) {
      return;
   }
  
   for (int i = 0; i < data.length(); ++i){
    if(String(data[i]) != "" || String(data[i]) != " "){
      EEPROM.write(i + pos, data[i]);
      Serial.print("write: ");
      Serial.println(data[i]); 
    }  
   };
};

String readDataFromEeprom(int from, int to, String name){
   Serial.print("Read data ");
   Serial.print(name);
   Serial.println(" eeprom.");

  String value = "";

  for (int i = from; i < to; ++i)
  {

    if(String(EEPROM.read(i)) != "" || String(EEPROM.read(i)) != " "){
      value += char(EEPROM.read(i));
    }
    
  };

  return value;
}

void setupEsp(){
  String ssidHomeWifi = server.arg("ssid");
  String password = server.arg("password");
  
  Serial.print("Name wifi:");
  Serial.println(ssidHomeWifi);
  Serial.print("Password:");
  Serial.println(password);

  if (ssidHomeWifi && password && ssidHomeWifi.length() > 0 && password.length() > 0) {
    Serial.println("Clean eeprom");
    cleanEEProm(0, maxSizeEeprom, false);
    addDataToEeprom(0, "NAME WIFI", ssidHomeWifi);
    addDataToEeprom(32, "PASS WIFI", password);
    EEPROM.commit();
  };
  
  ESP.reset();
}

bool checkWifi(void)
{
  int c = 0;
  Serial.println("Connecting: ");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    digitalWrite(13,HIGH);
    delay(250);
    digitalWrite(13,LOW);
    delay(250);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect wifi out of time !");
  return false;
};


void handleGetUser(){
  String isActice = Firebase.RTDB.getString(&fbdo, urlRequestGetUser) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

  Serial.printf("Pass handleGetUser"); 
  Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestGetUser, "false") ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestSetNewItem, "true") ? "ok" : fbdo.errorReason().c_str());

  String userId = Firebase.RTDB.getString(&fbdo, urlRequestSetUser) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

  if(userId && (userId != "" || userId != " ") && userId != "null" && userId != "connection lost" && userId != "false"){
    addDataToEeprom(96, "UserId", userId);
    EEPROM.commit();

    userIdGlobalValue = userId;

    Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestConnectedUser, "true") ? "ok" : fbdo.errorReason().c_str());

    Serial.print("Save User id success: ");
    Serial.println(userId);
  };
}

void setupFirebase () {
  Serial.println("Setup firebase !");
  config.api_key = api_key;
  auth.user.email = emailFirebaseAdmin;
  auth.user.password = passwordFirebaseAdmin;
  config.database_url = database_url;
  config.signer.tokens.legacy_token = tokenFirebase;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Success firebase !");
}

void setupLedStatus () {
  pinMode(D6, OUTPUT);
  pinMode(D5, INPUT);
  digitalWrite(D6,HIGH);
}

void setup() {
  pinMode(13,OUTPUT); //  green led
  pinMode(15,OUTPUT); //  red led
  EEPROM.begin(512); //Initialasing EEPROM
  Serial.begin(9600);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA); //(Set up a soft access point to establish a WiFi network)
  
  String nameWifi = "";
  String password = "";

  nameWifi = readDataFromEeprom(0, 32, "WIFI NAME");
  Serial.print("Name wifi: ");
  Serial.println(nameWifi);
  Serial.println();

  password = readDataFromEeprom(32, 96, "WIFI PASS");
  Serial.print("Password wifi: ");
  Serial.println(password);
  Serial.println();

  userIdGlobalValue = readDataFromEeprom(96, maxSizeEeprom, "User ID");
  Serial.print("UserId: ");
  Serial.println(userIdGlobalValue);
  Serial.println();

  WiFi.begin(nameWifi, password);

  setupLedStatus();
  
  if (checkWifi())
  {
    isConnectWifi = true;
    Serial.println("Connect successfully");
    Serial.print("IP:");
    Serial.println(WiFi.localIP());

    setupFirebase();
    
    server.on("/", connectIpEspWhenConnectSuccess);
    server.begin(); //Run server
    
    digitalWrite(13,HIGH);
    digitalWrite(15,LOW);
    
    return;
  }
  else {  
      isConnectWifi = false;
      
      digitalWrite(13,LOW);
      digitalWrite(15,HIGH);
      Serial.println("Setup wifi:");
      WiFi.softAP(ssidLocal, passwordLocal);
      Serial.print("Ip of ESP:");
      Serial.println(WiFi.softAPIP());
      server.on("/", connectIpEsp);
      server.on("/setup", setupEsp);
      server.begin();
  }
}

void handleTurnOnOff() {
    String isTurnOn = Firebase.RTDB.getString(&fbdo, urlRequestTurnOnOff) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

    if(isTurnOn == "true" && turnOnValue != isTurnOn) {
      Serial.println("Turn on");
      turnOnValue = isTurnOn;
      digitalWrite(D6, HIGH);
    };
    
    if(isTurnOn == "false" && turnOnValue != isTurnOn){
      Serial.println("Turn off");
      turnOnValue = isTurnOn;
       digitalWrite(D6, LOW);
    };
}

void handleResetUserIdEeprom() {
    String isReset = Firebase.RTDB.getString(&fbdo, urlRequestResetUserIdEeprom) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

    
    if(isReset == "true" && isReset != isResetEepromUserId){
      Serial.print("Reset userId in eeprom: ");
      Serial.println(isReset);
      
      isResetEepromUserId = isReset;
      
      Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestResetUserIdEeprom, "false") ? "Is reset userId in the eeprom" : fbdo.errorReason().c_str());

      if(isReset != "connection lost") {
        cleanEEProm(96, maxSizeEeprom);
      };
    };

    if(isReset == "false" && isReset != isResetEepromUserId){
      Serial.print("Reset userId in eeprom: ");
      Serial.println(isReset);
      isResetEepromUserId = isReset;
    };
}

void handleResetEeprom() {
    String isReset = Firebase.RTDB.getString(&fbdo, urlRequestResetEeprom) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

    
    if(isReset == "true" && isReset != isResetEepromUserId){
      Serial.print("Reset eeprom: ");
      Serial.println(isReset);
      
      isResetEepromUserId = isReset;
      
      Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestResetEeprom, "false") ? "Is reset userId in the eeprom" : fbdo.errorReason().c_str());

      if(isReset != "connection lost") {
        cleanEEProm(0, maxSizeEeprom);
      };
    };

    if(isReset == "false" && isReset != isResetEeprom){
      Serial.print("Reset eeprom: ");
      Serial.println(isReset);
      isResetEeprom = isReset;
    };
}

void handleCheckEspExistOnFirebase () {
    String urlCheckEspExistOnFirebase = "/" + idThisEsp;
    String checkEsp = Firebase.RTDB.getString(&fbdo, urlCheckEspExistOnFirebase) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

    Serial.println("check esp ...");
    Serial.println(checkEsp);
    isCheckExistEsp = true;
    if(checkEsp == "null") {
      cleanEEProm(0, 160);
    };
}

void handleLedD6Status () {
  bool button_pin5 = digitalRead(D5);
  if (button_pin5 == 0){
    digitalWrite(D6, LOW);
  }
  if (button_pin5 == 1){
    digitalWrite(D6, HIGH);
  }
}

void loop() {
 server.handleClient();
 handleLedD6Status();
 if(isConnectWifi){
    /*Reset user eeprom*/
    handleTurnOnOff();
    handleResetUserIdEeprom();
    handleResetEeprom();
    
    
    if(userIdGlobalValue == "" || userIdGlobalValue == " " || userIdGlobalValue == "null"){
      Serial.println("userIdValue ...");
      handleGetUser(); 
    }

    if(isCheckExistEsp == false) {
      handleCheckEspExistOnFirebase();
    }
  }
}
