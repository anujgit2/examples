#include <AceButton.h>
using namespace ace_button;
#define LED1 D1
#define LED2 D2
//#define LED3 D3
//#define LED4 D4
#define LED5 D5
#define LED6 D6
#define LED7 D7
static const uint8_t BUTTON_PIN = A0;
int sensorValue = 0;

// Create 4 AceButton objects, with their virtual pin number 0 to 3. The number
// of buttons does not need to be identical to the number of analog levels. You
// can choose to define only a subset of buttons here.
//
// Use the 4-parameter `AceButton()` constructor with the `buttonConfig`
// parameter explicitly to `nullptr` to prevent the automatic creation of the
// default SystemButtonConfig, saving about 30 bytes of flash and 26 bytes of
// RAM on an AVR processor.
static const uint8_t NUM_BUTTONS = 5;
static AceButton b0(nullptr, 0);
static AceButton b1(nullptr, 1);
static AceButton b2(nullptr, 2);
static AceButton b3(nullptr, 3);
static AceButton b4(nullptr, 4);

static AceButton* const BUTTONS[NUM_BUTTONS] = {
    &b0, &b1, &b2, &b3,&b4,
};

// Define the ADC voltage levels for each button.
// For 4 buttons, we need 5 levels.
static const uint8_t NUM_LEVELS = NUM_BUTTONS + 1;
static const uint16_t LEVELS[NUM_LEVELS] = {
  0 /* 0%, short to ground */,
    321 /* 32%, 4.7 kohm */,
  420 /* 32%, 4.7 kohm */,
  533 /* 50%, 10 kohm */,
  816 /* 82%, 47 kohm */,
  1023 /* 100%, 10-bit ADC, open circuit */,
};

// The LadderButtonConfig constructor binds the AceButton to the
// LadderButtonConfig.
static LadderButtonConfig buttonConfig(
  BUTTON_PIN, NUM_LEVELS, LEVELS, NUM_BUTTONS, BUTTONS
);

// The event handler for the button.
void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {

  // Print out a message for all events.
  Serial.print(F("handleEvent(): "));
  Serial.print(F("virtualPin: "));
  Serial.print(button->getPin());
  Serial.print(F("; eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);
  switch (eventType) {
    case AceButton::kEventReleased:
      /*if (button->getPin() == 0) {
        digitalWrite(LED1, LOW);
      }*/
      Serial.print(F("kEventReleased(): "));
      changeOutputStatus(button->getPin());
      break;
  }
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  digitalWrite(LED1, HIGH);// turn the LED off.(Note that LOW is the voltage level but actually 
digitalWrite(LED2, HIGH);
digitalWrite(LED5, HIGH);
digitalWrite(LED6, HIGH);
digitalWrite(LED7, HIGH);
  delay(1000); // some microcontrollers reboot twice
  Serial.begin(115200);
  while (! Serial); // Wait until Serial is ready - Leonardo/Micro
Serial.print(F("; buttonState: "));
  // Don't use internal pull-up resistor.
  pinMode(BUTTON_PIN, INPUT);
Serial.print(F("; buttonState: "));
  // Configure the ButtonConfig with the event handler, and enable all higher
  // level events.
buttonConfig.setEventHandler(handleEvent);
  buttonConfig.setFeature(ButtonConfig::kFeatureClick);
  //buttonConfig.setFeature(ButtonConfig::kFeatureDoubleClick);
  //buttonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  //buttonConfig.setFeature(ButtonConfig::kFeatureRepeatPress);
  Serial.print(F("; done setup: "));
}

// The buttonConfig.checkButtons() should be called every 4-5ms or faster, if
// the debouncing time is ~20ms. On ESP8266, analogRead() must be called *no*
// faster than 4-5ms to avoid a bug which disconnects the WiFi connection.
void checkButtons() {
  static unsigned long prev = millis();

  // DO NOT USE delay(5) to do this.
  unsigned long now = millis();
  if (now - prev > 50) {
    buttonConfig.checkButtons();
    prev = now;
  
  }
}
void changeOutputStatus(uint8_t button){
    Serial.print(F("; button pressed: "));
    Serial.print(button);
    if (button == 0) {
      Serial.print(F("; status: "));
      Serial.print(!digitalRead(LED1));
        digitalWrite(LED1, !digitalRead(LED1));
      }
      if (button == 1) {
        digitalWrite(LED2, !digitalRead(LED2));
      }
      if (button == 2) {
        digitalWrite(LED5, !digitalRead(LED5));
      }
      if (button == 3) {
        digitalWrite(LED6, !digitalRead(LED6));
      }
}
void loop() {
  checkButtons();
}
