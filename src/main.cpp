#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    64  // Updated for 2 displays
#define BRIGHTNESS  255 
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#define SEGMENT_COUNT 16
#define LEDS_PER_SEGMENT 2
#define DISPLAY_COUNT 2

#define RXD2 20
#define TXD2 21
HardwareSerial SerialBT(1);

// Generate segmentMap for 2 displays
int segmentMap[DISPLAY_COUNT][SEGMENT_COUNT][LEDS_PER_SEGMENT];

void generateSegmentMap() {
  for (int d = 0; d < DISPLAY_COUNT; d++) {
    for (int s = 0; s < SEGMENT_COUNT; s++) {
      for (int l = 0; l < LEDS_PER_SEGMENT; l++) {
        segmentMap[d][s][l] = d * SEGMENT_COUNT * LEDS_PER_SEGMENT + s * LEDS_PER_SEGMENT + l;
      }
    }
  }
}

// Character data (same as before, 0–9, A–Z, space)
int numbers[37][16] = {
  {1,1,0,0,1,1,1,0,1,1,0,1,0,1,1,0}, // 0
  {0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0}, // 1
  {1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0}, // 2
  {1,1,0,1,1,1,0,0,1,0,0,1,0,0,0,0}, // 3
  {0,0,1,1,0,0,1,0,1,0,0,1,0,0,0,0}, // 4
  {1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0}, // 5
  {1,1,1,1,1,1,1,0,0,1,0,1,0,0,0,0}, // 6
  {1,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1,0,1,1,0,1,0,0,0,0}, // 8
  {1,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0}, // 9
  {1,1,1,1,0,0,1,0,1,1,0,1,0,0,0,0}, // A
  {1,1,0,1,1,1,0,1,1,0,1,1,0,0,0,0}, // B
  {1,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0}, // C
  {1,1,0,0,1,1,0,1,1,0,1,1,0,0,0,0}, // D
  {1,1,1,0,1,1,1,0,0,1,0,0,0,0,0,0}, // E
  {1,1,1,0,0,0,1,0,0,1,0,0,0,0,0,0}, // F
  {1,1,0,1,1,1,1,0,0,1,0,1,0,0,0,0}, // G
  {0,0,1,1,0,0,1,0,1,1,0,1,0,0,0,0}, // H
  {1,1,0,0,1,1,0,1,0,0,1,0,0,0,0,0}, // I
  {0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0}, // J
  {0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,1}, // K
  {0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0}, // L
  {0,0,0,0,0,0,1,0,1,1,0,1,1,1,0,0}, // M
  {0,0,0,0,0,0,1,0,1,1,0,1,1,0,0,1}, // N
  {1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0}, // O
  {1,1,1,1,0,0,1,0,1,1,0,0,0,0,0,0}, // P
  {1,1,0,0,1,1,1,0,1,1,0,1,0,0,0,1}, // Q
  {1,1,1,1,0,0,1,0,1,1,0,0,0,0,0,1}, // R
  {1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0}, // S
  {1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0}, // T
  {0,0,0,0,1,1,1,0,1,1,0,1,0,0,0,0}, // U
  {0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,0}, // V
  {0,0,0,0,0,0,1,0,1,1,0,1,0,0,1,1}, // W
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1}, // X
  {0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,0}, // Y
  {1,1,0,0,1,1,0,0,0,0,0,0,0,1,1,0}, // Z
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}  // Space
};
String incomingText = "";

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(115200);
  SerialBT.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("ESP32-C3 ready. Send TEXT|COLOR over Bluetooth.");

  generateSegmentMap();
}

void clearAll() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void displayCharacter(char c, CRGB color, int displayIndex) {
  if (displayIndex < 0 || displayIndex >= DISPLAY_COUNT) return;
  int index;

  if (c == ' ') index = 36;
  else if (c >= '0' && c <= '9') index = c - '0';
  else if (c >= 'A' && c <= 'Z') index = c - 'A' + 10;
  else return;

  for (int seg = 0; seg < SEGMENT_COUNT; seg++) {
    if (numbers[index][seg]) {
      for (int i = 0; i < LEDS_PER_SEGMENT; i++) {
        leds[segmentMap[displayIndex][seg][i]] = color;
      }
    }
  }
}

CRGB parseHexColor(String hex) {
  long number = strtol(hex.c_str(), NULL, 16);
  uint8_t r = (number >> 16) & 0xFF;
  uint8_t g = (number >> 8) & 0xFF;
  uint8_t b = number & 0xFF;
  return CRGB(r, g, b);
}

void displayText(String text, CRGB color) {
  text = "  "+text + " "; // Padding for cleaner scrolling

  for (int i = 0; i < text.length(); i++) {
    clearAll();
    char left = (i < text.length()) ? toupper(text.charAt(i)) : ' ';
    char right = (i + 1 < text.length()) ? toupper(text.charAt(i + 1)) : ' ';
    displayCharacter(left, color, 0);
    displayCharacter(right, color, 1);
    FastLED.show();
    delay(500);
  }
}

void loop() {
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      incomingText.trim();
      int separator = incomingText.indexOf('|');
      if (separator != -1) {
        String text = incomingText.substring(0, separator);
        String hexColor = incomingText.substring(separator + 1);
        CRGB color = parseHexColor(hexColor);
        Serial.println("Displaying: " + text + " | #" + hexColor);
        displayText(text, color);
      }
      incomingText = "";
    } else {
      incomingText += c;
    }
  }
}
