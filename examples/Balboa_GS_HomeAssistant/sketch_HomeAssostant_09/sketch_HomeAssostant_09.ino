  /*Mode
 *    
 *    Main board: Wemos D1 mini - esp8266
 *  
 *    SPA display controller for Balboa system GS510DZ 
 *      Only temp working 2024-06-09
 *      Alfpha simple version
 */
    

#include <WiFiClient.h>
#ifdef ESP32
#include <WebServer.h>
#include <WiFi.h>
#else
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>  
#endif
#include <ElegantOTA.h>                  // https://github.com/ayushsharma82/ElegantOTA
#include <ArduinoHA.h>
#include <Balboa_GS_Interface.h>        // https://github.com/MagnusPer/Balboa-GS510SZ    

#ifdef ESP32
#define setClockPin D1 // CHANGE ME
#define setReadPin  D2 // CHANGE ME
#define setWritePin D8 // CHANGE ME 
#else
#define setClockPin D1  
#define setReadPin  D2 
#define setWritePin D8  
#endif

//Constants
const char *wifi_ssid                    = "wifi";          // WiFi SSID
const char *wifi_pwd                     = "pass";          // WiFi Password 
const char *wifi_hostname                = "SPA";
const char* mqtt_server                  = "192.168.1.1";           // MQTT Boker IP, your home MQTT server eg Mosquitto on RPi, or some public MQTT
const int mqtt_port                      = 1883;        // MQTT Broker PORT, default is 1883 but can be anything.
const char *mqtt_user                    = "addons";          // MQTT Broker User Name
const char *mqtt_pwd                     = "pass";          // MQTT Broker Password 

//Globals 
bool debug                               = false;    // If true activate debug values to write to serial port

const unsigned long ReportTimerMillis    = 1000;   // Timer in milliseconds to report mqtt topics

unsigned long ReportTimerPrevMillis      = 0;       // Store previous millis

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x32, 0x4A};  // Leave this value, unless you own multiple hot tubs

// MQTT Constants
const char* mqtt_Subscribe_write_topic      = "SPA/Write"; 
const char* mqtt_Subscribe_updateTemp_topic = "SPA/UpdateTemp";

//Initialize components
WiFiClient espClient;                                           // Setup WiFi client definition WiFi
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(espClient, device, 30);
BalboaInterface Balboa(setClockPin, setReadPin, setWritePin);   // Setup Balboa interface 
#ifdef ESP32
WebServer server(80);
#else
ESP8266WebServer server(80);
#endif

//Sensors
HASensor display("Display");
HASensor SetTemp("SetTemp");      
HASensorNumber waterTemp("waterTemp", HANumber::PrecisionP1);

HABinarySensor heater("Heater");
HABinarySensor stdmode("StandardMode");

HABinarySensor ecomode("EcoMode");
HABinarySensor pump1("Pump1");

HABinarySensor Filter1("Filter1");
HABinarySensor Filter2("Filter2");
HABinarySensor Filtration("Filtration");

HABinarySensor BitTest("BitTest");

HABinarySensor pump2("Pump2");
HABinarySensor blower("Blower");

HABinarySensor lights("Lights");

//Buttons
HAButton pump1Button("Pump1");
HAButton pump2Button("Pump2");
HAButton blowerButton("Blower");
HAButton lightsButton("Lights");

HAButton upButton("TempUp");
HAButton downButton("TempDown");

HAButton ButtonUpButton("ButtonUp");
HAButton ButtonDownButton("ButtonDown");



/**************************************************************************/
/* Setup                                                                  */
/**************************************************************************/

void setup() {
  
  if (debug) { Serial.begin(115200); Serial.println("Welcome to SPA - Balboa system GS510DZ");}
  setup_wifi();
  setup_HA();
  Serial.begin(115200);
  Balboa.begin();

  server.on("/", []() {
    server.send(200, "text/plain", "Hi! I am ESP8266 PCB bord 2 test3.");
  });
  
  ElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  if (debug) { Serial.println("HTTP server started"); }

}


/**************************************************************************/
/* Setup WiFi connection                                                  */
/**************************************************************************/

void setup_wifi() {

    /*  WiFi status return values and meaning 
        WL_IDLE_STATUS      = 0,
        WL_NO_SSID_AVAIL    = 1,
        WL_SCAN_COMPLETED   = 2,
        WL_CONNECTED        = 3,
        WL_CONNECT_FAILED   = 4,
        WL_CONNECTION_LOST  = 5,
        WL_WRONG_PASSWORD   = 6,
        WL_DISCONNECTED     = 7 */
  
    if (debug){ Serial.print("WiFi.status(): "); Serial.println(WiFi.status()); }
    
    int WiFi_retry_counter = 0;
    WiFi.mode(WIFI_STA);
    WiFi.hostname(wifi_hostname);
    #ifdef ESP32
    WiFi.setSleep(false);
    #else
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    #endif
    WiFi.begin(wifi_ssid, wifi_pwd);
    
    // Loop until reconnected or max retry then restart
    while (WiFi.status() != WL_CONNECTED){
        WiFi_retry_counter ++;
        if (WiFi_retry_counter == 30) {ESP.restart();}  
        if (debug){ Serial.print("WiFi.status(): "); Serial.print(WiFi.status()); 
                    Serial.print("   WiFi retry: "); Serial.println(WiFi_retry_counter); } 
        delay(1000);
    }
    
    if (debug){ Serial.print("WiFi connected: ");Serial.println(WiFi.localIP());}

}
// Need to clear & reconnect MQTT divice to see changes here

void setup_HA() {
    device.setName("Balboa SPA");
    device.setSoftwareVersion("0.90");
    device.setManufacturer("Balboa");
    device.setModel("GS510DZ");

    waterTemp.setName("Water temperature");
    waterTemp.setUnitOfMeasurement("°C");
    waterTemp.setDeviceClass("temperature");

    SetTemp.setName("Set Temp");
    SetTemp.setUnitOfMeasurement("°C");
    SetTemp.setDeviceClass("temperature");

    display.setName("Display");
    heater.setName("Heater");
    pump1.setName("Pump1");
     
    BitTest.setName("BitTest");

    Filter1.setName("Filter1");
    Filter2.setName("Filter2");
    Filtration.setName("Filtration");

    pump1Button.setName("Pump1");
    pump1Button.onCommand(onButtonPress);

    pump2.setName("Pump2");
//    pump2.onCommand(onSwitchCommand);
    pump2Button.setName("Pump2");
    pump2Button.onCommand(onButtonPress);

    blower.setName("Blower");
    blowerButton.setName("Blower");
    blowerButton.onCommand(onButtonPress);

    lights.setName("Lights");
    lightsButton.setName("Lights");
    lightsButton.onCommand(onButtonPress);

    upButton.setName("TempUp");
    upButton.onCommand(onButtonPress);
    
    downButton.setName("TempDown");
    downButton.onCommand(onButtonPress);
    
    ButtonUpButton.setName(" ButtonUp");
    ButtonUpButton.onCommand(onButtonPress);

    ButtonDownButton.setName("ButtonDown");
    ButtonDownButton.onCommand(onButtonPress);

    stdmode.setName("STD Mode");
    ecomode.setName("ECO Mode");
  

        mqtt.begin(mqtt_server, mqtt_user, mqtt_pwd);


}

/**************************************************************************/
/* Main loop                                                              */
/**************************************************************************/

void loop() {
  ElegantOTA.loop();
  Balboa.loop();
  mqtt.loop();
  server.handleClient();
  
  if (WiFi.status() != WL_CONNECTED){ setup_wifi(); }             // Check WiFi connnection reconnect otherwise 
 
    if(millis() - ReportTimerPrevMillis  > ReportTimerMillis) {
    
          ReportTimerPrevMillis = millis();
          
          display.setValue(Balboa.LCD_display.c_str());
          waterTemp.setValue(Balboa.waterTemperature);
          SetTemp.setValue(String(Balboa.setTemperature).c_str());           // Get Set TEMP
          stdmode.setState(Balboa.StandardMode);                             // Get STD mode
          ecomode.setState(Balboa.EcoMode);
          heater.setState(Balboa.Heater);                                    // Get heater status
          pump1.setState(Balboa.Pump1);
          pump2.setState(Balboa.Pump2);
          blower.setState(Balboa.Blower);
          lights.setState(Balboa.Lights);
          Filter1.setState(Balboa.Filter1);
          Filter2.setState(Balboa.Filter2);
          Filtration.setState(Balboa.Filtration);
          BitTest.setState(Balboa.displayBit55);                             //Test bit    
    } 
     
}

/**************************************************************************/
/* Subscribe to MQTT topic                                                */
/**************************************************************************/

void onButtonPress(HAButton* sender) {
  
    // Handling incoming messages

    Serial.println(sender->getName());

    String s_payload = sender->getName();
         
             if (s_payload == "ButtonUp") {
                  Balboa.writeDisplayData = true; 
                  Balboa.writeButtonUp      = true;
                  Serial.println(Balboa.setTemperature);
             }
             else if (s_payload == "ButtonDown") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeButtonDown    = true;  
                  Serial.println(Balboa.setTemperature);
             }
             else if (s_payload == "TempDown") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeTempDown    = true;  
             }
             else if (s_payload == "TempUp") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeTempUp    = true;  
             }
             else if (s_payload == "Lights") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeLights      = true; 
             }
             else if (s_payload == "Pump1") {
                  Balboa.writeDisplayData = true;
                  Balboa.writePump1       = true;  
             }
            // else if (s_payload == "Pump2") {
            //    Balboa.writeDisplayData = true;
            //     Balboa.writePump2       = true;  
            //}
             else if (s_payload == "Blower") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeBlower      = true;  
             }
             else if (s_payload == "STDMode") {
                  Balboa.writeDisplayData = true;
                //  Balboa.writeSTDMode     = true;  
             }
            else if (s_payload == "EcoMode") {
                  Balboa.writeDisplayData = true;
                 // Balboa.writeEcoMode     = true;  
             }
             else if (s_payload == "Stop") {
                  Balboa.stop();
             }
             else if (s_payload == "Reset") {
                ESP.restart();
             }
      
}
