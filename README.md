# ESP32_NVS_Web
 This project is basically a webserver build on ESP32 ,ESP32 connects to the network in two modes in Access point(AP) and Station mode (STA).In the webserver esp32 puts an webpage from the SPIFFS storage , writing an separate HTML page is simple than writing the html content in to the main code.

 # Learnings from the project:
     1. How to use SPIFFS.
     2. How to store WiFi credentials into NVS storage.
     3. Async webserver and DNS for the local webpage.
     4. using fetch api to post and get data.
 
# Features :
     The main HTML page for the webserver is stored into SPIFFS storage.
     Webpage contains a form to fill the network credentials and it has four buttons 
     About the buttons 
     Button 1. To delete the Stored credentials from the NVS storage.\n
     Button 2. To toggle the led turning **(ON/OFF).**
     BUtton 3. To print the ** "Hello_I_am_ESP32" ** in the Serial monitor and if you have attached the 16x2 LCD display , in that it displays ** "Hello from ESP32" ** for 2s.
     Button 4. This button does not connect to esp32 it on shows the weather of Canada. I tought of getting the weather from the website and using the ESp32 as server to publish it in the local webppage.
 # steps to setup the code to work:
 1. Download all the neccessary libraries from the library manager of Platfom io
      --> EspAsyncWebServer
      --> Preference insted of using this lib defaullt method can be used.
      --> LiquidCrystal_I2C for driving the LCD display.
    and all other libraries used are inbuilt library
 2. To setup the spiffs
      1.click the platform io extention icon
      2. It opens the project tasks , in this see for Platform
      3. In platform first click
              ----> Build FileSystem Image
              ----> Upload FileSystem Image
      4. Now upload the main.c file into ESP32 then it works perfectly.
