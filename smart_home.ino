#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>


//Global value
bool isConnectWifi = false;
String presubDataFirebase = "/esp/";
String userIdGlobalValue = "";
String codeGlobalValue = "";

// FIREBAE CONFIG
#define PROJECT_ID "htcdt-iot"

const char *api_key = "7234a488911316bedff11c65c28f361954a97051";
const char *emailFirebaseAdmin = "vuthanhhieu00@gmail.com";
const char *passwordFirebaseAdmin = "vuthanhhieu2000";
const char *database_url = "https://htcdt-iot-default-rtdb.firebaseio.com/";
const char *tokenFirebase = "KbjQ9IW5NEZhcikz8mA4B5LYHWWYQJo3wOYnUWyp";

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
///////////////////


const char *ssidLocal = "hieuesp";
const char *passwordLocal = "123456789";

ESP8266WebServer server(80); // khởi tạo sever ở port 80
WiFiManager wifiManager;

const char MAIN_page[] PROGMEM = R"=====(
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP8266 kết nối WiFi bằng điện thoại</title>
</head>
<body>

<form action="setup">
  Tên wifi: <input name="ssid" type="text"/> <br><br>
  Mật khẩu: <input name="password" type="text"/> <br>
  Code: <input name="code" type="text"/> <br>
  UserId: <input name="userId" type="text"/> <br>
  <input type="submit"/>
</form>
  


</body>

</html>
)=====";

void connectIpEsp(){
  server.send(200,"text/html",MAIN_page); // Send HTML
}

void connectIpEspWhenConnectSuccess(){
  server.send(200,"text/html", "Helloword"); // Send HTML
}

void cleanEEProm (int isReset){
  Serial.println("Start delete eeprom: ");
  Serial.print(isReset);

  if(isReset == 1) {
     for (int i = 0; i < 200; ++i) {
        EEPROM.write(i, 0);
     };
     
     EEPROM.commit();
     wifiManager.resetSettings();

     Serial.println("Delete data eeprom success");
  };
  
  delay(3000);
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
  String code = server.arg("code");
  String userId = server.arg("userId");
  
  Serial.print("Name wifi:");
  Serial.println(ssidHomeWifi);
  Serial.print("Password:");
  Serial.println(password);
  Serial.print("Code:");
  Serial.println(code);
  Serial.print("UserId:");
  Serial.println(userId);
  
  if (ssidHomeWifi.length() > 0 && password.length() > 0) {
    Serial.println("Clean eeprom");
    cleanEEProm(1);
    addDataToEeprom(0, "NAME WIFI", ssidHomeWifi);
    addDataToEeprom(32, "PASS WIFI", password);
    addDataToEeprom(96, "CODE WIFI", code);
    addDataToEeprom(160, "CODE WIFI", userId);
    
    EEPROM.commit();
  };
  
  ESP.reset();

//  server.send(200,"text/plain","đã kết nối thành công");
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
}

void setupFirebase () {
  Serial.println("Setup firebase: ");
  config.api_key = api_key;
  auth.user.email = emailFirebaseAdmin;
  auth.user.password = passwordFirebaseAdmin;
  config.database_url = database_url;
  config.signer.tokens.legacy_token = tokenFirebase;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Success firebase: ");
}

void setup() {
  pinMode(13,OUTPUT); //  green led
  pinMode(15,OUTPUT); //  red led
  EEPROM.begin(512); //Initialasing EEPROM
  Serial.begin(9600);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA); //(Set up a soft access point to establish a WiFi network)


  cleanEEProm(0);
  
  String nameWifi = "";
  String password = "";
  String code = "";
  String userId = "";
  
  nameWifi = readDataFromEeprom(0, 32, "WIFI NAME");
  Serial.print("Name wifi: ");
  Serial.println(nameWifi);
  Serial.println();

  password = readDataFromEeprom(32, 96, "WIFI PASS");
  Serial.print("Password wifi: ");
  Serial.println(password);
  Serial.println();

  code = readDataFromEeprom(96, 160, "CODE");
  codeGlobalValue = code;
  Serial.print("Code esp: ");
  Serial.println(code);
  Serial.println();

  userId = readDataFromEeprom(160, 200, "USER_ID");
  userIdGlobalValue = userId;
  Serial.print("User Id: ");
  Serial.println(userId);
  Serial.println();


  WiFi.begin(nameWifi, password);
  
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

void loop() {
 server.handleClient();

 if(isConnectWifi){
  Serial.println("Connect firebase: ");
  String uriFirebase = presubDataFirebase + codeGlobalValue;

  Serial.println(uriFirebase);
  
  Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  uriFirebase, "Hello word") ? "ok" : fbdo.errorReason().c_str());

  Serial.printf("Get string... %s\n", Firebase.RTDB.getString(&fbdo, uriFirebase) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
  delay(5000);
 }
}
