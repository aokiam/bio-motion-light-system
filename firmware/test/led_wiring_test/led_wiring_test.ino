// WS2812B strip test — solid colors, individual pixel order, brightness ramp.
// Confirms wiring, chain order, and color channel mapping before building
// the pulse/slash animations on top of it.
//
// Library needed: Adafruit NeoPixel

#include <Adafruit_NeoPixel.h>

#define LED_PIN     26
#define NUM_LEDS    25

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  delay(500);
  strip.begin();
  strip.show(); // all off initially
  Serial.println("WS2812B test starting...");
}

void loop() {
  // ---- Test 1: solid colors, all pixels at once ----
  Serial.println("Test 1: solid RED - every pixel should light, same color");
  colorWipeAll(strip.Color(100, 0, 0));
  delay(1500);

  Serial.println("Test 1: solid GREEN");
  colorWipeAll(strip.Color(0, 100, 0));
  delay(1500);

  Serial.println("Test 1: solid BLUE");
  colorWipeAll(strip.Color(0, 0, 100));
  delay(1500);

  // ---- Test 2: one pixel at a time, in chain order ----
  Serial.println("Test 2: lighting each pixel individually, in chain order");
  strip.clear();
  strip.show();
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.clear();
    strip.setPixelColor(i, strip.Color(0, 80, 255)); // blue
    strip.show();
    Serial.print("Pixel index "); Serial.print(i); Serial.println(" should be the only one lit");
    delay(400);
  }
  strip.clear();
  strip.show();
  delay(1000);

  // ---- Test 3: brightness ramp, all pixels ----
  Serial.println("Test 3: brightness ramp up then down, all pixels blue");
  for (int b = 0; b <= 255; b += 5) {
    setAllBrightness(0, 80, 255, b);
    strip.show();
    delay(15);
  }
  for (int b = 255; b >= 0; b -= 5) {
    setAllBrightness(0, 80, 255, b);
    strip.show();
    delay(15);
  }

  delay(1000); // pause before repeating the whole sequence
}

void colorWipeAll(uint32_t color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void setAllBrightness(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  float scale = brightness / 255.0;
  uint32_t color = strip.Color((uint8_t)(r * scale), (uint8_t)(g * scale), (uint8_t)(b * scale));
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
}