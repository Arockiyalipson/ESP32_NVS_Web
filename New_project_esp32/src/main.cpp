#include <Arduino.h>
#include "SPIFFS.h"
#include"ESPAsyncWebServer.h"
#include"AsyncTCP.h"
#include"AsyncJson.h"
#include "ESPmDNS.h"
#include"nvs.h"
#include"nvs_flash.h"
#include"Preferences.h"
#include"LiquidCrystal_I2C.h"

#define filepath "/index.html"  //file path vaiable 
#define led 2 //inbuilt led pin

//Variables storing wifi credentials for access point setup
const char* AP_ssid ="ENTER_YOUR_SSID";
const char* AP_pass = "ENTER_YOUR_PASSWORD";

const int I2C_SDA = 21;
const int I2C_SCL = 22;

// intialize server
AsyncWebServer server(80);

//intialize for nvs storage
Preferences pref;

//intitalize LCD
LiquidCrystal_I2C lcd(0x27,16,2);


// handle notopen response
void handlenotopen(AsyncWebServerRequest *request)
{
  if (request->method() == HTTP_OPTIONS)
    {
      request->send(200);
    }
    else
    {
      Serial.println("Not found");
      request->send(404, "Not found");
    }
}

// function to get WiFi-credentials from the client side form
void handlerequest(AsyncWebServerRequest *request)
{
  String STA_ssid = request->arg("SSID");
  String STA_pass = request->arg("pass");
  //printing SSID and PASSWORD
  Serial.println(STA_ssid);
  Serial.println(STA_pass);
  //store network credentials into NVS storage
  if(STA_ssid.length() !=0)
  {
    pref.begin("cred",false);
    pref.putString("ssid",STA_ssid);
    pref.putString("pass",STA_pass);
    pref.end();
    request->send(200,"OK");
    request->redirect("/");
  
    delay(1000);
    Serial.println("EPS restarting !!!!!!!!!");
    ESP.restart();
  }
  else
  {
    request->send(400,"Bad request");
  }

}

//function to connect to WiFi
void connect_WiFi()
{
  String NVS_ssid;
  String NVS_pass;
  pref.begin("cred",false);
  NVS_ssid =pref.getString("ssid","");
  NVS_pass = pref.getString("pass","");
  Serial.println(NVS_ssid);
  Serial.println(NVS_pass);
  if((NVS_ssid.length() !=3) && (NVS_pass.length() >= 8))
  {
    WiFi.begin(NVS_ssid.c_str(),NVS_pass.c_str());
    while(WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(500);
    }
    Serial.println("");
    Serial.print("IP Address: \t");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("No credentials found !!!!!!!!!!!!");
  }
}

void setup() {
  Serial.begin(115200);
  

  pinMode(led,OUTPUT);
  nvs_flash_init();
  
  // intializing lcd display
  lcd.init(I2C_SDA,I2C_SCL);
 
  // AP mode to publish website
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_ssid,AP_pass);
  Serial.print(" \nLocal IP address:");
  Serial.println(WiFi.softAPIP()); 
  
  MDNS.begin("Esp_server"); //DNS for ESP32 webserver
  //To access the webpage by putting this into the search bar
  Serial.print("Enter the DNS to get the webpage:");
  Serial.println("esp_server.local") ;

  

  //Checking for opening of SPIFFS
  if(SPIFFS.begin(true))
  {
    Serial.println("Sucessfully mounted");
  }
  else
  {
    Serial.println("error in opening SPIFFS file system");
  }


  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST,PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  // Handler to post the website to the client from SPIFFS storage
  server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(SPIFFS,filepath,"text/html");
  });

  //Handler for getting the WiFi_credentials data 
  server.on("/get",HTTP_POST,handlerequest);
  //connect to WiFi
  connect_WiFi();

  //Handler for getting button press for led
  server.addHandler(new AsyncCallbackJsonWebHandler("/led", [](AsyncWebServerRequest *request, JsonVariant &json) {
    const JsonObject &jsonObj = json.as<JsonObject>();
    if (jsonObj["on"])
    {
      Serial.println("Turn on LED");
      digitalWrite(led, HIGH);
    }
    else
    {
      Serial.println("Turn off LED");
      digitalWrite(led, LOW);
    }
    request->send(200, "OK");
  }));

  //Handler for getting button press for printing in Serial monitor 
  server.addHandler(new AsyncCallbackJsonWebHandler("/print", [](AsyncWebServerRequest *request, JsonVariant &json) {
    const JsonObject &jsonObj = json.as<JsonObject>();
    jsonObj["print_serial"];

    Serial.println("Hello_I_am_esp32");
    lcd.print("Hello from ESP32");
    delay(2000);
    lcd.clear();
    request->send(200, "OK");
  }));

  //Handler for getting button press for printing in Serial monitor 
  server.addHandler(new AsyncCallbackJsonWebHandler("/delete", [](AsyncWebServerRequest *request, JsonVariant &json) {
    const JsonObject &jsonObj = json.as<JsonObject>();
    
    jsonObj["NVS_del"];
    
    pref.begin("cred",false);
    pref.clear();
    Serial.println("NVS values has deleted");
    pref.end();
    Serial.println("ESP restarting ...........");
    delay(1000);
    ESP.restart();
    request->send(200,"OK");
    
  }));
  
  //handler for 404 error for website 
  server.onNotFound(handlenotopen);
  //Starting the server
  server.begin();
  Serial.println("Server started");
}
 
void loop() {
}