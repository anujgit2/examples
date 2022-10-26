#include <EEPROM.h>
  #include <AceButton.h>
  using namespace ace_button;
  #include <ShiftRegister74HC595.h>
   #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
  
  #include <uri/UriBraces.h>
  #include <uri/UriRegex.h>
  #include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266NetBIOS.h>
  
  /*
   *  create a global shift register object
      parameters: <number of shift registers> 
      (data pin DS-14, clock pin SHCP-11, latch pin STCP-12)
      NODE MCU PIN  | SHIFT REGISTER PIN |  NAME  |  DESCRIPION
      --------------------------------------------------------
      D3            | 14                 |  DS    | DATA PIN 
      --------------------------------------------------------
      D0            | 11                 |  SH_CP | CLOCK PIN  
      --------------------------------------------------------
      D4            | 12                 |  ST_CP | LATCH PIN
      --------------------------------------------------------
   */
  ShiftRegister74HC595<1> sr(D3, D0, D4);
  ESP8266WebServer server(80);

  const int LED_ON = LOW;
  const int LED_OFF = HIGH;
    
  // Number of buttons and LEDs.
  const uint8_t NUM_LEDS = 6;
  const uint8_t INDICATOR_LED = 7;
  // Helper struct that keeps track of the buttons and leds. An alternative is to
  // use multiple arrays (e.g. BUTTON_PINS, LED_PINS, LED_STATES) which has the
  // potential of reduing memory using PROGMEM, but for small to medium number of
  // buttons, using a struct makes the program easier to read.
  struct Info {
    const uint8_t buttonPin;
    const uint8_t ledPin;
    bool ledState;
  };
    
  // An array of button pins, led pins, and the led states. Cannot be const
  // because Info.ledState is mutable.
  // ShiftRegister output button 0 is not in use right now
  Info INFOS[NUM_LEDS] = {
    {D1, 1, LED_OFF},
    {D2, 2, LED_OFF},
    {D5, 3, LED_OFF},
    {D6, 4, LED_OFF},
    {D7, 5, LED_OFF},
    {D8, 6, LED_OFF},
  };

  AceButton buttons[NUM_LEDS];

  // Forward reference to prevent Arduino compiler becoming confused.
  void handleEvent(AceButton*, uint8_t, uint8_t);
  

  
  unsigned int  timeout   = 10; // seconds to run for
  unsigned int  apTimeout   = 120; // hotspot will be started for 2 minute
  unsigned int  apStartTime = millis();
  unsigned int  startTime = millis();
  
  bool apStarted = false; // start AP and webserver if true, else start only webserver
  byte value;
  bool wifiConnected = false;
  bool serverStarted = false;
  bool hotspotStarted = false;
  
  String strChipId = String(ESP.getChipId());
  
  IPAddress local_IP(192,168,3,1);
  IPAddress gateway(192,168,3,9);
  IPAddress subnet(255,255,255,0);
  
  void setup() {

    Serial.begin(115200);
    while (! Serial); // Wait until Serial is ready - Leonardo/Micro
    
    buttonSetup();
    
    EEPROM.begin(NUM_LEDS);
    
    WiFi.mode(WIFI_STA);
    //WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.begin();
  
    //set the last saved switch status, 
    for(int i=0;i<NUM_LEDS;i++) {
      
      value = EEPROM.read(i);
      uint8_t currentState = uint8_t(value);
      
      if(currentState <= 1) {
         
          INFOS[i].ledState = currentState;
          sr.set(i+1, currentState);
          delay(200);
      }
      
    }
    
  }


  
  void loop() {
    
    checkButtons();
    checkWifiConnection();
    
    if (serverStarted) {
      server.handleClient();
      MDNS.update();
    }
    
  }

  String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

  void checkWifiConnection() {
    if((millis()-startTime) > (timeout*1000)){
      Serial.println("wifi check : "+String(WiFi.status()));
      if (WiFi.status() == WL_CONNECTED) {
          Serial.println("");
          Serial.println("WiFi connected");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
          if (!serverStarted) {
            serverSetup();
            serverStarted = true;
            Serial.println("Server started");
            MDNS.begin(strChipId);
            MDNS.addService("http", "tcp", 80);
           
          }
        }

        startTime = millis(); 
       }
       
   

   //check access point open duration
   if ( apStarted ) {
    
       if((millis()-apStartTime) > (apTimeout*1000))  {
      
        closeHotSpot();
        apStarted = false;
       
       }
   }
}

void openWIFIAccessPoint() {
  
        Serial.print("Going to open access point....with default ssid and password...");
        WiFi.disconnect(true);
        delay(3000);
       
        WiFi.mode(WIFI_AP_STA);//client and access point
        
        String ssid = "HM" + strChipId;
        String password="12345678";
        
        Serial.println(ssid);
        Serial.println(password);
        WiFi.softAPConfig(local_IP, gateway, subnet);
        WiFi.softAP(ssid.c_str(), password.c_str());
        WiFi.begin();
        delay(1000);
        Serial.println(WiFi.softAPIP());
        sr.set(7, LED_ON);
        
    }
     void closeHotSpot() {
       
        WiFi.softAPdisconnect();
        delay(1000);
        sr.set(7, LED_OFF);
        Serial.println("HotSpot is closed.");
        WiFi.disconnect(true);
        delay(3000);
        WiFi.mode(WIFI_STA);//client and access point
        WiFi.begin();
        
    }
