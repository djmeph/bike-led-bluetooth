#include <Arduino.h>
#include <BluetoothSerial.h>
#include <FastLED.h>
#include <Bounce2.h>

#define BUTTON_LED_PIN          25
#define BUTTON_PIN              32
#define CH1_PIN                 12
#define CH2_PIN                 27
#define LEDS_PER_CHANNEL        82
#define BRIGHTNESS              32
#define UPDATES_PER_SECOND      100
#define LED_TYPE                WS2811
#define COLOR_ORDER             GRB

CRGB          ch1Leds[LEDS_PER_CHANNEL];
CRGB          ch2Leds[LEDS_PER_CHANNEL];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
BluetoothSerial SerialBT;
Bounce bounce = Bounce();
byte BTData;
byte currentSetting;
byte lastSetting;
uint8_t brightness = 255;

/* Check if Bluetooth configurations are enabled in the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void light(uint8_t colorIndex) {
  for (int i = 0; i < LEDS_PER_CHANNEL; i++) {
    ch1Leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    ch2Leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void dark() {
  for (int i = 0; i < LEDS_PER_CHANNEL; i++) {
    ch1Leds[i] = CRGB::Black;
    ch2Leds[i] = CRGB::Black;
  }
}

void setup() {
  Serial.begin(9600);
  SerialBT.begin("UnicornCycle");
  Serial.println("Bluetooth Started! Ready to pair...");
  pinMode(BUTTON_LED_PIN, OUTPUT);
  digitalWrite(BUTTON_LED_PIN, HIGH);
  bounce.attach( BUTTON_PIN ,  INPUT_PULLUP );
  bounce.interval(10);
  delay(1000);
  FastLED.addLeds<LED_TYPE, CH1_PIN, COLOR_ORDER>(ch1Leds, LEDS_PER_CHANNEL).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, CH2_PIN, COLOR_ORDER>(ch2Leds, LEDS_PER_CHANNEL).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  currentSetting = '1';
}

void loop() {
  bounce.update();

  if (bounce.changed()) {
    int deboucedInput = bounce.read();

    if ( deboucedInput == HIGH ) {
      switch (currentSetting) {
        case '1':
        currentSetting = '2';
        break;
        case '2':
        currentSetting = '3';
        break;
        case '3':
        currentSetting = '4';
        break;
        case '4':
        currentSetting = '5';
        break;
        case '5':
        currentSetting = '6';
        break;
        case '6':
        currentSetting = '0';
        break;
        case '0':
        currentSetting = '1';
        break;
      }
    }
  }

  if(SerialBT.available()) {
    BTData = SerialBT.read();
    if (BTData) {
      if (
        BTData == '1' ||
        BTData == '2' ||
        BTData == '3' ||
        BTData == '4' ||
        BTData == '5' ||
        BTData == '6' ||
        BTData == '0'
      ) currentSetting = BTData;
    }
  }

  if (currentSetting != lastSetting) {
    switch (currentSetting) {
      case '1':
      currentPalette = LavaColors_p;
      currentBlending = LINEARBLEND;
      SerialBT.print("Lava Colors - Linear Blend\n");
      break;
      case '2':
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
      SerialBT.print("Rainbow Colors - Linear Blend\n");
      break;
      case '3':
      currentPalette = CloudColors_p;
      currentBlending = NOBLEND;
      SerialBT.print("Cloud Colors - No Blend\n");
      break;
      case '4':
      currentPalette = OceanColors_p;
      currentBlending = LINEARBLEND;
      SerialBT.print("Ocean Colors - Linear Blend\n");
      break;
      case '5':
      currentPalette = ForestColors_p;
      currentBlending = LINEARBLEND;
      SerialBT.print("Forest Colors - Linear Blend\n");
      break;
      case '6':
      currentPalette = PartyColors_p;
      currentBlending = NOBLEND;
      SerialBT.print("Party Colors - No Blend\n");
      break;
    }
  }

  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;

  if (currentSetting == '0' && currentSetting != lastSetting) SerialBT.print("Off\n");
  if (currentSetting == '0') dark();
  else light(startIndex);

  lastSetting = currentSetting;

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}
