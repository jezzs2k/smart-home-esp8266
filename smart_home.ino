#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <SD.h>
#include <EEPROM.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include "HTML.h"
#include <PZEM004Tv30.h>

/*Setup PZEM*/
/* Use software serial for the PZEM
   Pin 4 Rx (Connects to the Tx pin on the PZEM)
   Pin 5 Tx (Connects to the Rx pin on the PZEM)
*/
#define PZEM_RX_PIN 4
#define PZEM_TX_PIN 5
PZEM004Tv30 pzem(PZEM_RX_PIN, PZEM_TX_PIN);

//Variable delay real time
unsigned long t1 = 0;

/*------------------------------------------------------*/

//{"ssid":"SMART_HOME_ESP8266","password":"11111111","idEsp":"b0706e84-4513-11ec-81d3-0242ac130005"}
/*DEFAULT_VALUE*/
String idThisEsp = "";
const char *ssidLocal = "SMART_HOME_ESP8266";
int maxSizeEeprom = 512;
int timeReconnect = 0;
/*------------------------------------------------------*/

/*Global value*/
bool isConnectedEspWithFirebase = false;
bool isSetupURL = false;
bool isConnectWifi = false;
bool isFirebaseConnected = false;
String presubDataFirebase = "/esp/";
String userIdGlobalValue = "";
String turnOnValue = "";
String isResetEepromUserId = "";
String isResetEeprom = "";
bool isCheckExistEsp = false;
String nameWifi = "";
String password = "";
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
String urlRequestGetUser = "";
String urlRequestGetUserError = "";
String urlRequestSetNewItem = "";
String urlRequestSetUser = "";
String urlRequestConnectedUser = "";
String urlRequestResetUserIdEeprom = "";
String urlRequestResetEeprom = "";
String urlRequestTurnOnOff = "";
String urlCheckEspExistOnFirebase = "";
String urlRequestEnrgy = "";
String urlSetEnrgy = "";
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
     delay(5);
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
    if(String(char(data[i])) != "" || String(char(data[i])) != " "){
      EEPROM.write(i + pos, data[i]);
      Serial.print("write: ");
      Serial.println(data[i]);
      delay(5); 
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

    if(String(char(EEPROM.read(i))) != "" /*|| String(EEPROM.read(i)) != " "*/){
      value += String(char(EEPROM.read(i)));
    }
    delay(10);
  };

  return value;
}

void setupEsp(){
  String ssidHomeWifi = server.arg("ssid");
  String password = server.arg("password");
  String id = server.arg("idThisEsp");
  
  Serial.print("Name wifi:");
  Serial.println(ssidHomeWifi);
  Serial.print("Password:");
  Serial.println(password);

  if (ssidHomeWifi && password && id && ssidHomeWifi.length() > 0 && password.length() > 0 && id.length() > 0) {
    Serial.println("Clean eeprom");
    cleanEEProm(0, maxSizeEeprom, false);
    addDataToEeprom(0, "NAME WIFI", ssidHomeWifi);
    addDataToEeprom(32, "PASS WIFI", password);
    addDataToEeprom(96, "Id ESP", id);
    idThisEsp = id;
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
  Serial.println(isActice); 
  if(isActice == "true") {
    Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestGetUser, "false") ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestSetNewItem, "true") ? "ok" : fbdo.errorReason().c_str());

     String userId = Firebase.RTDB.getString(&fbdo, urlRequestSetUser) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

    if(userId && (userId != "" || userId != " ") && userId != "null" && userId != "connection lost" && userId != "false"){
      Serial.print("Save User id success: ");
      Serial.println(userId);
      addDataToEeprom(160, "UserId", userId);
  
      userIdGlobalValue = userId;
      EEPROM.commit();
    };
  }
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
  isFirebaseConnected = true;
}

void setupWifi(){
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  nameWifi = readDataFromEeprom(0, 32, "WIFI NAME");
  Serial.print("Name wifi: ");
  Serial.println(nameWifi);
  Serial.println();

  password = readDataFromEeprom(32, 96, "WIFI PASS");
  Serial.print("Password wifi: ");
  Serial.println(password);
  Serial.println();

  idThisEsp = readDataFromEeprom(96, 160, "ID ESP");
  Serial.print("Is ESP: ");
  Serial.println(idThisEsp);
  Serial.println();

  userIdGlobalValue = readDataFromEeprom(160, 250, "User ID");
  Serial.print("UserId: ");
  Serial.println(userIdGlobalValue);
  Serial.println();


  WiFi.begin(nameWifi, password);  
  timeReconnect ++;

  if(timeReconnect > 11 || nameWifi == ""){
    Serial.print("TimeReconnect:");
    Serial.println(timeReconnect);
    timeReconnect = 0;
    isConnectWifi = false;
    digitalWrite(13,LOW);
    digitalWrite(15,HIGH);
    Serial.println("Setup wifi:");
    WiFi.softAP(ssidLocal);
    Serial.print("Ip of ESP:");
    Serial.println(WiFi.softAPIP());
    server.on("/", connectIpEsp);
    server.on("/setup", setupEsp);
    server.begin();
  }
}

void checkWifiSetup(){
    Serial.print("checkWifiSetup:");
    Serial.println(timeReconnect);
  if (checkWifi())
  {
    isConnectWifi = true;
    Serial.println("Connect successfully");
    Serial.print("IP:");
    Serial.println(WiFi.localIP());
    
    server.on("/", connectIpEspWhenConnectSuccess);
    server.begin(); //Run server
    
    digitalWrite(13,HIGH);
    digitalWrite(15,LOW);
    
    return;
  }
  else {  
      isConnectWifi = false;
      setupWifi();
  }
}

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);

  setupWifi();
}

void handleTurnOnOff() {
    String isTurnOn = Firebase.RTDB.getString(&fbdo, urlRequestTurnOnOff) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

    if(isTurnOn == "true"){
      dataElectricityMeter(); 
    };

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
    String checkEsp = Firebase.RTDB.getString(&fbdo, urlCheckEspExistOnFirebase) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    handleCheckConnected();
    Serial.println("check esp ...");
    Serial.println(checkEsp);
    isCheckExistEsp = true;
    
    if(checkEsp == "null") {
      cleanEEProm(0, maxSizeEeprom);
    };
}

void handleSetupURL(String idEsp) {
  String id = "";
  for(int i = 0; i < 36; ++i){
    id += String(idEsp[i]);
  }
  urlRequestSetNewItem =  "/" + id + "/isNewItem";
  urlRequestGetUser =  "/" + id + "/isActive";
  urlRequestGetUserError = "/" + id + "/isError";
  urlRequestSetUser =  "/" + id + "/setUser";
  urlRequestConnectedUser =  "/" + id + "/isConnected";
  urlRequestResetUserIdEeprom =  "/" + id + "/isResetUserIdEeprom";
  urlRequestResetEeprom =  "/" + id + "/isResetEeprom";
  urlRequestTurnOnOff = "/" + id + "/isTurnOn";
  urlCheckEspExistOnFirebase = "/" + id;
  urlRequestEnrgy = "/" + id + "/isResetEnergy";
  urlSetEnrgy = "/" + id + "/energy";
  isSetupURL = true;
  Serial.println("Setup url ok");
  Serial.println(id.length());
}

void handleCheckConnected(){
  String isConnected1 = Firebase.RTDB.getString(&fbdo, urlRequestConnectedUser) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();

  if(isConnected1 == "true"){
    isConnectedEspWithFirebase = true;
  }else {
    Serial.printf("Set string... %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestConnectedUser, "true") ? "ok" : fbdo.errorReason().c_str()); 
  };
   
}

/*Format JSON*/
String formatJsonString(String key, int value, String src){  
  src = src + key + ":" + (String)value + ",";

  return src;
}

void saveDataElectricityMerer(String src){
  Serial.println(src);

  Serial.printf("Set string.Enrgy %s\n", Firebase.RTDB.setString(&fbdo,  urlSetEnrgy, src) ? "ok" : fbdo.errorReason().c_str());   
}


/*PZEM*/
// float voltage = 0, current = 0, power = 0, energytage = 0, pf = 0, frequency = 0, electricityBill = 0;
void dataElectricityMeter(){
  String src = "{";

  // src = formatJsonString("voltage", voltage++, src);
  // src = formatJsonString("current", current++, src);
  // src = formatJsonString("power", power++, src);
  // src = formatJsonString("energytage", energytage++, src);
  // src = formatJsonString("pf", pf++, src);
  // src = formatJsonString("frequency", frequency++, src);
  // src = formatJsonString("electricityBill", electricityBill++, src);
  // src = src + "}";

  // Serial.println(src);

  // Serial.printf("Set string.Enrgy %s\n", Firebase.RTDB.setString(&fbdo,  urlSetEnrgy, src) ? "ok" : fbdo.errorReason().c_str()); 

  // delay(1000);
      
 if( millis() - t1 > 2000){
   Serial.print("Custom Address:");
   Serial.println(pzem.readAddress(), HEX);
   
   //Reset the data of energy

   
   // Read the data from the sensor
   
   float voltage = pzem.voltage();
   float current = pzem.current();
   float power = pzem.power();
   float energy = pzem.energy();
   float frequency = pzem.frequency();
   float pf = pzem.pf();
   
   double electricityBill = 0;
   unsigned int bac1 = 1678, bac2 = 1734, bac3 = 2014, bac4 = 2536, bac5 = 2834, bac6 = 2927;
  
   if(energy <= 50){
     electricityBill += energy*bac1;
   }
   else if(energy <= 100){
     electricityBill += (energy - 50)*bac2 + 50*bac1;
   }   
   else if(energy <= 200){
     electricityBill += (energy - 100)*bac3 + 50*bac2 + 50*bac1;
   }
   else if(energy <= 300){
     electricityBill += (energy - 200)*bac4 + 100*bac3 + 50*bac2 + 50*bac1;
   }
   else if(energy <= 400){
     electricityBill += (energy - 300)*bac5 + 100*bac4 + 100*bac3 + 50*bac2 + 50*bac1;
   }
   else{
     electricityBill += (energy - 400)*bac6 + 100*bac5 + 100*bac4 + 100*bac3 + 50*bac2 + 50*bac1;
   }

   // Check if the data is valid
   if (isnan(voltage)) {
     Serial.println("Error reading voltage");
   } else if (isnan(current)) {
     Serial.println("Error reading current");
   } else if (isnan(power)) {
     Serial.println("Error reading power");
   } else if (isnan(energy)) {
     Serial.println("Error reading energy");
   } else if (isnan(frequency)) {
     Serial.println("Error reading frequency");
   } else if (isnan(pf)) {
     Serial.println("Error reading power factor");
   } else {
     src = formatJsonString("voltage", voltage, src);
     src = formatJsonString("current", current, src);
     src = formatJsonString("power", power, src);
     src = formatJsonString("energytage", energy, src);
     src = formatJsonString("pf", pf, src);
     src = formatJsonString("frequency", frequency, src);
     src = formatJsonString("electricityBill", electricityBill, src);
     src = src + "}";

     saveDataElectricityMerer(src);
     
     Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
     Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
     Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
     Serial.print("Energy: ");       Serial.print(energy, 3);     Serial.println("kWh");
     Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
     Serial.print("PF: ");           Serial.println(pf);
     Serial.print("Electricity bill: ");    Serial.print(electricityBill, 0); Serial.println("VND");
   }
   Serial.println();
   t1 = millis();
 }
}

void handleResetEnergy(){
    String isResetEnergy = Firebase.RTDB.getString(&fbdo, urlRequestEnrgy) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    if(isResetEnergy == "true"){
        pzem.resetEnergy();
        Serial.printf("Set string.Reset enrgy %s\n", Firebase.RTDB.setString(&fbdo,  urlRequestEnrgy, "false") ? "ok" : fbdo.errorReason().c_str()); 
    }
}


void loop() {
 server.handleClient();
 Serial.print("WiFiStatus-");
 Serial.println(WiFi.status());
 Serial.print("WL_CONNECTED-");
 Serial.println(WL_CONNECTED);

 Serial.print("nameWifi-");
 Serial.println(nameWifi.length());
  Serial.print("password-");
 Serial.println(password.length());

 if(WiFi.status() != WL_CONNECTED && nameWifi.length() > 1 && password.length() > 1){
   checkWifiSetup();
   delay(1000);
 }
 
 if(isConnectWifi){
    timeReconnect = 0;
    if(isFirebaseConnected) {
      if(isSetupURL == false) {
        handleSetupURL(idThisEsp);  
      };
      
      if(isSetupURL){
        handleTurnOnOff();
        handleResetEeprom();  
        handleResetEnergy();

       if(isCheckExistEsp == false) {
          handleCheckEspExistOnFirebase();
        }
      };
     }

    if(idThisEsp != "" && isFirebaseConnected == false){
      setupFirebase();
    }
  }
}
