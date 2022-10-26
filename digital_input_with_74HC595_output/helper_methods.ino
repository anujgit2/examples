
void serverSetup() {
  server.on(F("/"), []() {
    
    String ip_address = "\"ip\":\""+String(WiFi.localIP()[0])+"."+WiFi.localIP()[1]+"."+WiFi.localIP()[2]+"."+ WiFi.localIP()[3]+"\"";
    String ap_ip = "\"ap-ip\":\""+String(WiFi.softAPIP()[0])+"."+WiFi.softAPIP()[1]+"."+WiFi.softAPIP()[2]+"."+ WiFi.softAPIP()[3]+"\"";
    String result = "{ ";
    result += "\"chip_id\":\""+strChipId+"\"";
  
    result += ", \"mac\":\""+ WiFi.macAddress()+"\"";
    result += ", \"ssid\":\""+WiFi.SSID()+"\"";
    result += ", \"connected\":"+String(WiFi.isConnected());    
     result += ", \"switch-count\":"+String(NUM_LEDS);    
    result += ", "+ip_address;
    result += ", "+ap_ip;
    result += " }";
  
    server.send(200, "application/json", result);
  });
  
 
  server.on(UriBraces("/{}/switch/{}/{}"), []() {
    String key = server.pathArg(0);//chipid as string do validate with chip id and incoming key, return if not match
    String button = server.pathArg(1);
    String state = server.pathArg(2);
    changeStatus(button.toInt(),state.toInt());
    server.send(200, "text/plain", "Switch: '" + button + "'" +" Status: '" + state + "'");
  });
  
 server.on("/save-wifi", [](){
            String key = server.arg("key");//chipid as string do validate with chip id and incoming key, return if not match
            String ssid = server.arg("name");
            String pass = server.arg("password");
           
             WiFi.begin(ssid.c_str(), pass.c_str());
            delay(2000);
            String status ="";
            if(WiFi.status()==WL_CONNECTED){
                String ip_address = "\"ip_address\":\""+String(WiFi.localIP()[0])+"."+WiFi.localIP()[1]+"."+WiFi.localIP()[2]+"."+ WiFi.localIP()[3]+"\"";
    
                status = "{\"ip\":\""+ip_address+"\",\"mac\":\""+WiFi.macAddress()+"\", \"status\":true}";
            }
            else{
             
                status = "{\"ip\":\"\",\"mac\":\""+WiFi.macAddress()+"\", \"status\":false}";
            }
            Serial.println(status);
            server.send(200, "application/json", status);
        
        });
  
  server.begin();
  Serial.println("HTTP server started");
}

  // The buttonConfig.checkButtons() should be called every 4-5ms or faster, if
  // the debouncing time is ~20ms. On ESP8266, analogRead() must be called *no*
  // faster than 4-5ms to avoid a bug which disconnects the WiFi connection.
  void checkButtons() {
    static unsigned long prev = millis();
    unsigned long now = millis();
    // DO NOT USE delay(5) to do this.
    
    if (now - prev > 20) {
      
     for (uint8_t i = 0; i < NUM_LEDS; i++) {
        buttons[i].check();
      }
      //Serial.println(String(WiFi.isConnected()));
      prev = now;
    
    }
  }

  void buttonSetup()  {
    
    uint8_t pinValues[] = { B11111111 }; //default off, low level relay trigger
    sr.setAll(pinValues); 
    
   for(int i=0;i<NUM_LEDS;i++){
      value = EEPROM.read(i);
      uint8_t currentState = uint8_t(value);
      if(currentState > 1){
        EEPROM.write(i, byte(LED_OFF));
        INFOS[i].ledState = LED_OFF;
      }
     
    }
    
    EEPROM.commit();
    
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
     
      pinMode(INFOS[i].buttonPin, INPUT);
      
      buttons[i].init(INFOS[i].buttonPin, LOW, i);
     
    }
    
    // Configure the ButtonConfig with the event handler, and enable all higher
    // level events.
    ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setClickDelay(1000);
    buttonConfig->setDebounceDelay(60);
    buttonConfig->setLongPressDelay(5000);
    buttonConfig->setFeature(ButtonConfig::kFeatureClick);
    buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
    buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
    
  }

  // The event handler for the button.
  void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  
    // Get the LED pin
    uint8_t id = button->getId();
    uint8_t ledPin = INFOS[id].ledPin;
    switch (eventType) {
      
      case AceButton::kEventClicked:    {
          Serial.print(F("; kEventClicked button: "));
          Serial.println(id);
          uint8_t state = !INFOS[id].ledState;
          changeStatus(id,state);
        }
        break;
     case AceButton::kEventPressed:    {
          Serial.print(F("; pressed button: "));
          Serial.println(id);
          sr.set(INDICATOR_LED, LED_ON);
          //uint8_t state = !INFOS[id].ledState;
          //changeStatus(id,state);
        }
        break;
     case AceButton::kEventReleased:    {
          Serial.print(F("; kEventReleased: "));
          Serial.println(id);
          sr.set(INDICATOR_LED, LED_OFF);
          //uint8_t state = !INFOS[id].ledState;
          //changeStatus(id,state);
        }
        break;
      case AceButton::kEventLongPressed:   { //not working for now
          Serial.print(F("; long pressed button:  "));
          Serial.println(id);
          //startConfigPortal();
          if(!apStarted)  {
              openWIFIAccessPoint();
              apStartTime = millis();
              apStarted = true;
          }
        }
        break;
    
    }
    
  }

    void changeStatus(uint8_t buttonId, uint8_t state){
        uint8_t ledPin = INFOS[buttonId].ledPin;
        sr.set(ledPin, state);
        INFOS[buttonId].ledState = state;
        EEPROM.write(buttonId, byte(state));
        EEPROM.commit();
       
    }
 
