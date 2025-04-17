#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    32
#define BRIGHTNESS  10 
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
#define SEGMENT_COUNT 16
#define LEDS_PER_SEGMENT 2

#define RXD2 20
#define TXD2 21
HardwareSerial SerialBT(1);

int segmentMap[SEGMENT_COUNT][LEDS_PER_SEGMENT] = {
  {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13}, {14, 15},
  {16, 17}, {18, 19}, {20, 21}, {22, 23}, {24, 25}, {26, 27}, {28, 29}, {30, 31}
};

// Character data (0-9, A-Z, space) – same as before
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
}

void clearAll() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void displayCharacter(char c, CRGB color) {
  clearAll();
  int index;
  if (c == ' ') index = 36;
  else if (c >= '0' && c <= '9') index = c - '0';
  else if (c >= 'A' && c <= 'Z') index = c - 'A' + 10;
  else return;

  for (int seg = 0; seg < SEGMENT_COUNT; seg++) {
    if (numbers[index][seg]) {
      for (int i = 0; i < LEDS_PER_SEGMENT; i++) {
        leds[segmentMap[seg][i]] = color;
      }
    }
  }
  FastLED.show();
}

void displayCustomPattern(int customPattern[16], CRGB color) {
  clearAll();
  
  for (int seg = 0; seg < SEGMENT_COUNT; seg++) {
    if (customPattern[seg]) {
      for (int i = 0; i < LEDS_PER_SEGMENT; i++) {
        leds[segmentMap[seg][i]] = color;
      }
    }
  }
  FastLED.show();
}

CRGB parseHexColor(String hex) {
  long number = strtol(hex.c_str(), NULL, 16);
  uint8_t r = (number >> 16) & 0xFF;
  uint8_t g = (number >> 8) & 0xFF;
  uint8_t b = number & 0xFF;
  return CRGB(r, g, b);
}

void displayText(String text, CRGB color) {
  for (int i = 0; i < text.length(); i++) {
    displayCharacter(toupper(text.charAt(i)), color);
    delay(500);
  }
}

void loop() {
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      incomingText.trim();
      
      // Check if this is a custom pattern command
      if (incomingText.startsWith("CUSTOM|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        
        if (secondSeparator != -1) {
          String patternStr = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor = incomingText.substring(secondSeparator + 1);
          
          // Parse the pattern
          int customPattern[16];
          for (int i = 0; i < 16 && i < patternStr.length(); i++) {
            customPattern[i] = (patternStr.charAt(i) == '1') ? 1 : 0;
          }
          
          CRGB color = parseHexColor(hexColor);
          Serial.println("Displaying custom pattern with color #" + hexColor);
          displayCustomPattern(customPattern, color);
        }
      } else {
        // Handle regular text|color command
        int separator = incomingText.indexOf('|');
        if (separator != -1) {
          String text = incomingText.substring(0, separator);
          String hexColor = incomingText.substring(separator + 1);
          CRGB color = parseHexColor(hexColor);
          Serial.println("Displaying: " + text + " | #" + hexColor);
          displayText(text, color);
        }
      }
      incomingText = "";
    } else {
      incomingText += c;
    }
  }
}
