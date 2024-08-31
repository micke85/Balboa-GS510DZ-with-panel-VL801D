/*
Realse version 1.0
 */
#ifdef ESP32
#include <WebServer.h>
#include <WiFi.h>
#else
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>  
#endif
#include <WiFiClient.h>
#include <ElegantOTA.h>                 // https://github.com/ayushsharma82/ElegantOTA
#include <ArduinoHA.h>
#include <Balboa_GS_Interface.h>        //https://github.com/micke85/Balboa-GS510DZ-with-panel-VL801D/

#ifdef ESP32
#define setClockPin 0 // CHANGE TO WHAT YOU HAVE, IF USING ESP32
#define setReadPin  0 // CHANGE TO WHAT YOU HAVE, IF USING ESP32
#define setWritePin 0 // CHANGE TO WHAT YOU HAVE, IF USING ESP32
#else // if using 8266
#define setClockPin D1 // D1 Acording to PCB board BY MagnusPer/Balboa-GS510SZ    
#define setReadPin  D2 // D2 Acording to PCB board BY MagnusPer/Balboa-GS510SZ
#define setWritePin D8 // D8 Acording to PCB board BY MagnusPer/Balboa-GS510SZ
#endif

//Constants
const char *wifi_ssid                    = "";         // WiFi SSID
const char *wifi_pwd                     = "";       // WiFi Password 
const char *wifi_hostname                = "SPA";             // Name of the unit, you dont need to change this.
const char* mqtt_server                  = "";   // MQTT Boker IP, your home MQTT server eg Mosquitto on RPi, or some public MQTT
const int mqtt_port                      = 1883;              // MQTT Broker PORT, default is 1883 but can be anything.
const char *mqtt_user                    = "";          // MQTT Broker User Name
const char *mqtt_pwd                     = "";      // MQTT Broker Password 
const char* ReleaseVersion               = "1.0";       // Release Version latest version you will find at https://github.com/micke85/Balboa-GS510DZ-with-panel-VL801D

//Globals 
bool debug                               = false;      // If true activate debug values to write to serial port

const unsigned long ReportTimerMillis    = 500;       //  Timer in milliseconds to report mqtt topics
unsigned long ReportTimerPrevMillis      = 0;          // Store previous millis





byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x32, 0x4B};  // Leave this value, unless you own multiple hot tubs

// MQTT Constants
const char* mqtt_Subscribe_write_topic      = "SPA/Write"; 
const char* mqtt_Subscribe_updateTemp_topic = "SPA/UpdateTemp";



//Initialize components
WiFiClient espClient;                                           // Setup WiFi client definition WiFi
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(espClient, device, 34);                             // Max amount devices as sensors, buttons MQTT
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
HABinarySensor START("START");
HABinarySensor STOP("STOP");

HABinarySensor BitTest("BitTest");

HABinarySensor TempMenu("TempMenu");
HABinarySensor timeMenu("TimeMenu");
HABinarySensor modeProg("ModeProg");  

HABinarySensor AM("AM");
HABinarySensor PM("PM");

HABinarySensor pump2("Pump2");
HABinarySensor blower("Blower");

HABinarySensor lights("Lights");

//Switches
HASwitch pump1Switch("Pump1");      
HASwitch pump2Switch("Pump2");   
HASwitch blowerSwitch("Blower");   
HASwitch lightsSwitch("Lights");   

//Buttons
HAButton timeMenuButton("TimeMenu");
HAButton modeProgButton("ModeProg");
HAButton DownButton("Down");
HAButton UpButton("Up");
HAButton TempUpButton("TempUp");
HAButton TempDownButton("TempDown");



/**************************************************************************/
/* Setup                                                                  */
/**************************************************************************/

void setup() {
  
  if (debug) { Serial.begin(115200); Serial.println("Welcome to Balboa GS510DZ");}
  setup_wifi();
  setup_HA();
  Serial.begin(115200);
  Balboa.begin();
  
  server.on("/", []() {
  String response = "Hi! I am ESP8266, SPA version " + String(ReleaseVersion);
  server.send(200, "text/plain", response);
  // server.send(200, "text/plain", "Hi! I am ESP8266, SPA version ", ReleaseVersion);
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
// Need to reconnect MQTT divice to see changes here

void setup_HA() {
    device.setName("Balboa SPA");
    device.setSoftwareVersion(ReleaseVersion);
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
     
    BitTest.setName("BitTest");
    TempMenu.setName("TempMenu");

    AM.setName("AM");
    PM.setName("PM");

    Filter1.setName("Filter1");
    Filter2.setName("Filter2");
    Filtration.setName("Filtration");
    START.setName("START");
    STOP.setName("STOP");

    pump1.setName("Pump1");
    pump1Switch.setName("Pump1");
    pump1Switch.onCommand(onSwitchCommand);

    pump2.setName("Pump2");
    pump2Switch.setName("Pump2");
    pump2Switch.onCommand(onSwitchCommand);

    blower.setName("Blower");
    blowerSwitch.setName("Blower");
    blowerSwitch.onCommand(onSwitchCommand);

    lights.setName("Lights");
    lightsSwitch.setName("Lights");
    lightsSwitch.onCommand(onSwitchCommand);

    timeMenu.setName("TimeMenu");
    timeMenuButton.setName("TimeMenu");
    timeMenuButton.onCommand(onButtonPress);

    modeProg.setName("ModeProg");
    modeProgButton.setName("ModeProg");
    modeProgButton.onCommand(onButtonPress);

    TempUpButton.setName("TempUp");
    TempUpButton.onCommand(onButtonPress);
    
    TempDownButton.setName("TempDown");
    TempDownButton.onCommand(onButtonPress);
    
    UpButton.setName("Up");
    UpButton.onCommand(onButtonPress);

    DownButton.setName("Down");
    DownButton.onCommand(onButtonPress);

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
          //Display status
          display.setValue(Balboa.LCD_display.c_str());
          waterTemp.setValue(Balboa.waterTemperature);
          SetTemp.setValue(String(Balboa.setTemperature).c_str());       
          stdmode.setState(Balboa.StandardMode);                      // Get STD mode
          ecomode.setState(Balboa.EcoMode);
          heater.setState(Balboa.Heater);                             // Get heater status
          pump1.setState(Balboa.Pump1);
          pump2.setState(Balboa.Pump2);
          blower.setState(Balboa.Blower);
          lights.setState(Balboa.Lights);
          Filter1.setState(Balboa.Filter1);
          Filter2.setState(Balboa.Filter2);
          START.setState(Balboa.START);
          STOP.setState(Balboa.STOP);
          Filtration.setState(Balboa.Filtration);
          BitTest.setState(Balboa.displayBit62);                       //Test bit   intressitng to test still 62 and 43
          timeMenu.setState(Balboa.TimeMenu);                          //time menu status
          TempMenu.setState(Balboa.TempMenu);                          //Temp menu status   
          modeProg.setState(Balboa.ModeProg);                          //Mode/prog
          AM.setState(Balboa.displayAM);         
          PM.setState(Balboa.displayPM);                        
    } 
     
}

/**************************************************************************/
/* Subscribe to MQTT topic                                                */
/**************************************************************************/

//Switches
void onSwitchCommand(bool state, HASwitch* sender) {
  //String name = sender->getName();
  String s_payload = sender->getName();

  if(s_payload == "Pump1") {
    if(Balboa.Pump1 != state) {
      Balboa.writeDisplayData = true;
      Balboa.writePump1       = true;  
    }
  }
  else if(s_payload == "Pump2") {
    if(Balboa.Pump2 != state) {
      Balboa.writeDisplayData = true;
      Balboa.writePump2       = true;  
    }
  }
  else if(s_payload == "Blower") {
    if(Balboa.Blower != state) {
      Balboa.writeDisplayData = true;
      Balboa.writeBlower       = true;  
    }
  }
  else if(s_payload == "Lights") {
    if(Balboa.Lights != state) {
      Balboa.writeDisplayData = true;
      Balboa.writeLights       = true;  
    }
  }
  else {
    Serial.printf("Unknown switch %s\n", s_payload);
  }

}
//Buttons
void onButtonPress(HAButton* sender) {
  
    // Handling incoming messages

    Serial.println(sender->getName());

    String s_payload = sender->getName();
         
              if (s_payload == "Up") {     
                  Balboa.writeDisplayData = true;
                  Balboa.writeButtonUp    = true;  
             }
             else if (s_payload == "TempDown") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeTempDown    = true;  
             }
             else if (s_payload == "TempUp") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeTempUp      = true;  
             }
               else if (s_payload == "TimeMenu") {
                  Balboa.writeDisplayData = true;
                  Balboa.writeTimeMenu    = true;  
             }
                else if (s_payload == "ModeProg") {     
                  Balboa.writeDisplayData = true;
                  Balboa.writeModeProg    = true;  
             }
                else if (s_payload == "Down") {     
                  Balboa.writeDisplayData = true;
                  Balboa.writeButtonDown    = true;  
             }
             else if (s_payload == "STDMode") {
                  Balboa.writeDisplayData = true;
                //  Balboa.writeSTDMode   = true;  
             }
            else if (s_payload == "EcoMode") {
                  Balboa.writeDisplayData = true;
                 // Balboa.writeEcoMode   = true;  
             }
             else if (s_payload == "Stop") {
                  Balboa.stop();
             }
             else if (s_payload == "Reset") {
                ESP.restart();
             }
      
}