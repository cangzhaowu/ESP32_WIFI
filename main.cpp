#include <WiFi.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <WiFiAP.h>
String wifiName = "";
String wifiPassword = "";

const char* apSsid = "ESP32-Config"; // SSID
const char* apPassword = "password"; // password

AsyncWebServer server(80);

void handleRoot(AsyncWebServerRequest *request){
  String html = "<html><body>";
  html += "<h1>WIFI Settings</h1>";
  html += "<form action=\"/save\" method=\"POST\">";
  html += "WIFI_SSID: <input type=\"text\" name=\"wifiName\"><br>";
  html += "WIFI_Password: <input type=\"password\" name=\"wifiPassword\"><br>";
  html += "<input type=\"submit\" value=\"保存\">";
  html += "</form>";
  html += "</body></html>";

  request->send(200, "text/html", html);
}

void handleSave(AsyncWebServerRequest *request){
  if(request->hasParam("wifiName") && request->hasParam("wifiPassword")){
    String newWifiName = request->getParam("wifiName")->value();
    String newWifiPassword = request->getParam("wifiPassword")->value();

    // store SSID and password in EPROM
    for(int i=0; i<newWifiName.length(); i++){
      EEPROM.write(i, newWifiName[i]);
    }
    for(int i=0; i<newWifiPassword.length(); i++){
      EEPROM.write(10+i, newWifiPassword[i]);
    }
    EEPROM.commit();

    wifiName = newWifiName.c_str();
    wifiPassword = newWifiPassword.c_str();
  }

  String html = "<html><body>";
  html += "<h1>Setting success</h1>";
  html += "</body></html>";

  request->send(200, "text/html", html);
}

void setup(){
  EEPROM.begin(512); // EEPROM start

  // try to get Wifi passwd and ssid from eprom
  for(int i=0; i<10; i++){
    wifiName += char(EEPROM.read(i));
  }
  for(int i=0; i<10; i++){
    wifiPassword += char(EEPROM.read(10+i));
  }

  // set AP mode and start web server
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid, apPassword);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  server.begin();
}

void loop(){
  if(wifiName != "" && wifiPassword != ""){
    //try to connect
    WiFi.begin(wifiName.c_str(), wifiPassword.c_str());

    //wait
    while(WiFi.status() != WL_CONNECTED){
      delay(1000);
    }

   //type out status and IP
    Serial.print("Connected to WIFI: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  
  // ...
}