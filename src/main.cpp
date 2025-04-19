#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    64  // Doubled from 32 to support two displays
#define BRIGHTNESS   
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
#define SEGMENT_COUNT 16
#define LEDS_PER_SEGMENT 2
#define DISPLAY_COUNT 2
#define LEDS_PER_DISPLAY 32

#define RXD2 20
#define TXD2 21
HardwareSerial SerialBT(1);

// Updated to support two displays
int segmentMap[DISPLAY_COUNT][SEGMENT_COUNT][LEDS_PER_SEGMENT] = {
  // First display
  {
    {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13}, {14, 15},
    {16, 17}, {18, 19}, {20, 21}, {22, 23}, {24, 25}, {26, 27}, {28, 29}, {30, 31}
  },
  // Second display
  {
    {32, 33}, {34, 35}, {36, 37}, {38, 39}, {40, 41}, {42, 43}, {44, 45}, {46, 47},
    {48, 49}, {50, 51}, {52, 53}, {54, 55}, {56, 57}, {58, 59}, {60, 61}, {62, 63}
  }
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

// Function prototypes
void displayCharacter(char c, CRGB color, int displayIndex);
void displayText(String text, CRGB color, int displayIndex);
void displayCustomPattern(int customPattern[16], CRGB color, int displayIndex);
void scrollText(String text, CRGB color, int displayIndex, int scrollSpeed);
void scrollTextAcrossDisplays(String text, CRGB color, int scrollSpeed);

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(115200);
  SerialBT.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("ESP32-C3 ready. Send TEXT|COLOR|DISPLAY over Bluetooth.");
}

void clearAll() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

// Clear a specific display
void clearDisplay(int displayIndex) {
  for (int i = displayIndex * LEDS_PER_DISPLAY; i < (displayIndex + 1) * LEDS_PER_DISPLAY; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

// Modified to support display selection
void displayCharacter(char c, CRGB color, int displayIndex = 0) {
  clearDisplay(displayIndex);
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
  FastLED.show();
}

// Modified to support display selection
void displayCustomPattern(int customPattern[16], CRGB color, int displayIndex = 0) {
  clearDisplay(displayIndex);
  
  for (int seg = 0; seg < SEGMENT_COUNT; seg++) {
    if (customPattern[seg]) {
      for (int i = 0; i < LEDS_PER_SEGMENT; i++) {
        leds[segmentMap[displayIndex][seg][i]] = color;
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

// Modified to support scrolling text
void displayText(String text, CRGB color, int displayIndex = 0) {
  for (int i = 0; i < text.length(); i++) {
    displayCharacter(toupper(text.charAt(i)), color, displayIndex);
    delay(500);
  }
}

// New function for scrolling text across displays
void scrollText(String text, CRGB color, int displayIndex = 0, int scrollSpeed = 150) {
  if (text.length() == 0) return;
  
  // Convert to uppercase
  text.toUpperCase();
  
  // Add spaces at beginning and end for smooth scrolling
  text = "  " + text + "  ";
  
  // Create a virtual screen with characters
  int virtualLength = text.length();
  
  // Scroll through the text
  for (int startPos = 0; startPos < virtualLength - 1; startPos++) {
    clearDisplay(displayIndex);
    
    // Display current character
    char currentChar = text.charAt(startPos);
    displayCharacter(currentChar, color, displayIndex);
    
    delay(scrollSpeed);
  }
}

// Function to scroll text across both displays as one continuous display
void scrollTextAcrossDisplays(String text, CRGB color, int scrollSpeed = 150) {
  if (text.length() == 0) return;
  
  // Convert to uppercase
  text.toUpperCase();
  
  // Add spaces at beginning and end for smooth scrolling
  text = "  " + text + "  ";
  
  int virtualLength = text.length();
  
  // Scroll through the text, moving across both displays
  for (int startChar = 0; startChar < virtualLength; startChar++) {
    // Clear both displays
    clearAll();
    
    // Show current character on first display
    if (startChar < virtualLength) {
      displayCharacter(text.charAt(startChar), color, 0);
    }
    
    // Show next character on second display if available
    if (startChar + 1 < virtualLength) {
      displayCharacter(text.charAt(startChar + 1), color, 1);
    }
    
    delay(scrollSpeed);
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
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        
        if (secondSeparator != -1) {
          String patternStr = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor;
          int displayIndex = 0; // Default to first display
          
          if (thirdSeparator != -1) {
            hexColor = incomingText.substring(secondSeparator + 1, thirdSeparator);
            displayIndex = incomingText.substring(thirdSeparator + 1).toInt();
            if (displayIndex < 0 || displayIndex >= DISPLAY_COUNT) displayIndex = 0;
          } else {
            hexColor = incomingText.substring(secondSeparator + 1);
          }
          
          // Parse the pattern
          int customPattern[16];
          for (int i = 0; i < 16 && i < patternStr.length(); i++) {
            customPattern[i] = (patternStr.charAt(i) == '1') ? 1 : 0;
          }
          
          CRGB color = parseHexColor(hexColor);
          Serial.println("Displaying custom pattern with color #" + hexColor + " on display " + String(displayIndex));
          displayCustomPattern(customPattern, color, displayIndex);
        }
      } 
      // Check if this is a scrolling text command
      else if (incomingText.startsWith("SCROLL|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        int fourthSeparator = incomingText.indexOf('|', thirdSeparator + 1);
        
        if (secondSeparator != -1) {
          String text = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor;
          int scrollSpeed = 150; // Default speed
          
          if (thirdSeparator != -1) {
            hexColor = incomingText.substring(secondSeparator + 1, thirdSeparator);
            scrollSpeed = incomingText.substring(thirdSeparator + 1).toInt();
            if (scrollSpeed < 50) scrollSpeed = 50;
          } else {
            hexColor = incomingText.substring(secondSeparator + 1);
          }
          
          CRGB color = parseHexColor(hexColor);
          Serial.println("Scrolling text across displays: " + text + " | #" + hexColor + " with speed " + String(scrollSpeed));
          scrollTextAcrossDisplays(text, color, scrollSpeed);
        }
      } else {
        // Handle regular text|color command
        int firstSeparator = incomingText.indexOf('|');
        if (firstSeparator != -1) {
          String text = incomingText.substring(0, firstSeparator);
          String hexColor;
          int displayIndex = 0; // Default to first display
          
          int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
          if (secondSeparator != -1) {
            hexColor = incomingText.substring(firstSeparator + 1, secondSeparator);
            displayIndex = incomingText.substring(secondSeparator + 1).toInt();
            if (displayIndex < 0 || displayIndex >= DISPLAY_COUNT) displayIndex = 0;
          } else {
            hexColor = incomingText.substring(firstSeparator + 1);
          }
          
          CRGB color = parseHexColor(hexColor);
          Serial.println("Displaying: " + text + " | #" + hexColor + " on display " + String(displayIndex));
          displayText(text, color, displayIndex);
        }
      }
      incomingText = "";
    } else {
      incomingText += c;
    }
  }
}
