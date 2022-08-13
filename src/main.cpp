#include <Arduino.h>
#include <BluetoothSerial.h>
#include <FastLED.h>
#include <elapsedMillis.h>

#define CH1_PIN                 12
#define CH2_PIN                 18
#define CH1_LEDS_PER_CHANNEL    45
#define CH2_LEDS_PER_CHANNEL    8
#define BRIGHTNESS              32
#define LED_TYPE                WS2812
#define COLOR_ORDER             GRB

CRGB          ch1Leds[CH1_LEDS_PER_CHANNEL];
CRGB          ch2Leds[CH2_LEDS_PER_CHANNEL];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
BluetoothSerial SerialBT;
byte BTData;

elapsedMillis fps;
float monoPeak = 0.0;
int ch1PeakValue = 0;
int ch2PeakValue = 0;
int i = 0;
uint8_t brightness = 255;

/* Check if Bluetooth configurations are enabled in the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void light(uint8_t colorIndex) {
  for (i = 0; i < CH1_LEDS_PER_CHANNEL; i++) {
    ch1Leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
  }
  for (i = 0; i < CH2_LEDS_PER_CHANNEL; i++) {
    ch2Leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
  }
  FastLED.show();
}

void setup() {
  FastLED.addLeds<LED_TYPE, CH1_PIN, COLOR_ORDER>(ch1Leds, CH1_LEDS_PER_CHANNEL);
  FastLED.addLeds<LED_TYPE, CH2_PIN, COLOR_ORDER>(ch2Leds, CH2_LEDS_PER_CHANNEL);
  FastLED.setBrightness(BRIGHTNESS);
  currentPalette = LavaColors_p;
  currentBlending = LINEARBLEND;
  delay(1000);
  Serial.begin(9600);
  SerialBT.begin();
  Serial.println("Bluetooth Started! Ready to pair...");
}

void loop() {
  if(SerialBT.available()) {
    BTData = SerialBT.read();
    Serial.write(BTData);
  }

  switch (BTData) {
    case '1':
    currentPalette = LavaColors_p;
    break;
    case '2':
    currentPalette = RainbowColors_p;
    break;
    case '3':
    currentPalette = CloudColors_p;
    break;
    case '4':
    currentPalette = OceanColors_p;
    break;
    case '5':
    currentPalette = ForestColors_p;
    break;
    case '6':
    currentPalette = PartyColors_p;
    break;
  }

  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;
  light(startIndex);
}
