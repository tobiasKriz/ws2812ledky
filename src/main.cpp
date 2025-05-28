#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    64
#define BRIGHTNESS  100
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

// LED segment mapping
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

// Character data (0-9, A-Z, space)
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
String commandBuffer = "";
volatile bool stopFlag = false;

void displayCharacter(char c, CRGB color, int displayIndex);
void displayCustomPattern(int customPattern[16], CRGB color, int displayIndex);
void scrollTextAcrossDisplays(String text, CRGB color, int scrollSpeed, int repeatCount = 1);
void rainbowText(String text, int scrollSpeed, int repeatCount = 1);
void showAnimation(String animationType, CRGB color, int animSpeed = 100, int repeatCount = 1);
CRGB getHueColor(uint8_t hue);
CRGB parseHexColor(String hex);
bool delayWithStopCheck(int delayMs);
void clearAll();
void clearDisplay(int displayIndex);
void sendResponse(String message, bool success = true);
bool readBTCommand(String &command);

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  SerialBT.begin(9600, SERIAL_8N1, RXD2, TXD2);
  sendResponse("READY", true);
}

void sendResponse(String message, bool success) {
  String responseStr = success ? "OK:" : "ERROR:";
  responseStr += message + "\n";
  SerialBT.print(responseStr);
}

bool readBTCommand(String &command) {
  command = "";
  unsigned long startTime = millis();
  const unsigned long timeout = 100;
  
  while (millis() - startTime < timeout) {
    if (SerialBT.available()) {
      char c = SerialBT.read();
      if (c == '\n') {
        command.trim();
        return true;
      }
      command += c;
    }
    yield();
  }
  return false;
}

void clearAll() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void clearDisplay(int displayIndex) {
  for (int i = displayIndex * LEDS_PER_DISPLAY; i < (displayIndex + 1) * LEDS_PER_DISPLAY; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

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

void displayCustomPattern(int customPattern[16], CRGB color, int displayIndex) {
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

bool delayWithStopCheck(int delayMs) {
  const int checkInterval = 5;
  unsigned long startTime = millis();
  unsigned long endTime = startTime + delayMs;
  
  if (commandBuffer == "STOP") {
    stopFlag = true;
    clearAll();
    commandBuffer = "";
    sendResponse("STOPPED", true);
    return false;
  }
  
  while (millis() < endTime && !stopFlag) {
    if (SerialBT.available()) {
      while (SerialBT.available()) {
        char c = SerialBT.read();
        if (c == '\n') {
          commandBuffer.trim();
          if (commandBuffer == "STOP") {
            stopFlag = true;
            clearAll();
            commandBuffer = "";
            return false;
          }
          commandBuffer = "";
        } else {
          commandBuffer += c;
        }
      }
    }
    
    delay(min(checkInterval, (int)(endTime - millis())));
    yield();
  }
  
  return !stopFlag;
}

void scrollTextAcrossDisplays(String text, CRGB color, int scrollSpeed, int repeatCount) {
  if (text.length() == 0) return;
  stopFlag = false;
  
  text.toUpperCase();
  text = "  " + text + "  ";
  
  int virtualLength = text.length();
  int currentRepeat = 0;
  
  while ((currentRepeat < repeatCount || repeatCount == -1) && !stopFlag) {
    if (SerialBT.available()) {
      String command = "";
      while (SerialBT.available()) {
        char c = SerialBT.read();
        if (c == '\n') break;
        command += c;
      }
      
      command.trim();
      if (command == "STOP") {
        stopFlag = true;
        clearAll();
        break;
      }
    }
    
    for (int startChar = 0; startChar < virtualLength && !stopFlag; startChar++) {
      if (startChar < virtualLength) {
        displayCharacter(text.charAt(startChar), color, 0);
      }
      
      if (startChar + 1 < virtualLength) {
        displayCharacter(text.charAt(startChar + 1), color, 1);
      }
      
      if (!delayWithStopCheck(scrollSpeed)) {
        return;
      }
    }
    
    if (repeatCount != -1) {
      currentRepeat++;
    }
  }
}

void rainbowText(String text, int scrollSpeed, int repeatCount) {
  if (text.length() == 0) return;
  
  text.toUpperCase();
  text = "  " + text + "  ";
  
  int virtualLength = text.length();
  int currentRepeat = 0;
  
  stopFlag = false;
  
  uint8_t hue = 0;
  int displayDist = 64;
  int hueDelta = 2;
  
  while ((currentRepeat < repeatCount || repeatCount == -1) && !stopFlag) {
    for (int startChar = 0; startChar < virtualLength && !stopFlag; startChar++) {
      if (startChar < virtualLength) {
        displayCharacter(text.charAt(startChar), getHueColor(hue), 0);
      }
      
      if (startChar + 1 < virtualLength) {
        displayCharacter(text.charAt(startChar + 1), getHueColor(hue + displayDist), 1);
      }
      
      int colorUpdates = scrollSpeed / 15;
      
      for (int i = 0; i < colorUpdates && !stopFlag; i++) {
        int currentIndex;
        if (text.charAt(startChar) == ' ') currentIndex = 36;
        else if (text.charAt(startChar) >= '0' && text.charAt(startChar) <= '9') currentIndex = text.charAt(startChar) - '0';
        else if (text.charAt(startChar) >= 'A' && text.charAt(startChar) <= 'Z') currentIndex = text.charAt(startChar) - 'A' + 10;
        
        CRGB color1 = getHueColor(hue);
        for (int seg = 0; seg < SEGMENT_COUNT; seg++) {
          if (numbers[currentIndex][seg]) {
            for (int j = 0; j < LEDS_PER_SEGMENT; j++) {
              leds[segmentMap[0][seg][j]] = color1;
            }
          }
        }
        
        if (startChar + 1 < virtualLength) {
          if (text.charAt(startChar + 1) == ' ') currentIndex = 36;
          else if (text.charAt(startChar + 1) >= '0' && text.charAt(startChar + 1) <= '9') currentIndex = text.charAt(startChar + 1) - '0';
          else if (text.charAt(startChar + 1) >= 'A' && text.charAt(startChar + 1) <= 'Z') currentIndex = text.charAt(startChar + 1) - 'A' + 10;
          
          CRGB color2 = getHueColor(hue + displayDist);
          for (int seg = 0; seg < SEGMENT_COUNT; seg++) {
            if (numbers[currentIndex][seg]) {
              for (int j = 0; j < LEDS_PER_SEGMENT; j++) {
                leds[segmentMap[1][seg][j]] = color2;
              }
            }
          }
        }
        
        FastLED.show();
        hue += hueDelta;
        
        if (!delayWithStopCheck(15)) {
          return;
        }
      }
    }
    
    if (repeatCount != -1) {
      currentRepeat++;
    }
  }
}

CRGB getHueColor(uint8_t hue) {
  CHSV hsv(hue, 240, 255);
  CRGB rgb;
  hsv2rgb_rainbow(hsv, rgb);
  return rgb;
}

void showAnimation(String animationType, CRGB color, int animSpeed, int repeatCount) {
  stopFlag = false;
  
  if (repeatCount < -1) repeatCount = 1;
  int currentRepeat = 0;
  
  while ((currentRepeat < repeatCount || repeatCount == -1) && !stopFlag) {
    if (animationType.equals("WAVE")) {
      for (int cycle = 0; cycle < 3 && !stopFlag; cycle++) {
        for (int segment = 0; segment < SEGMENT_COUNT && !stopFlag; segment++) {
          clearAll();
          for (int display = 0; display < DISPLAY_COUNT; display++) {
            for (int i = 0; i < LEDS_PER_SEGMENT; i++) {
              leds[segmentMap[display][segment][i]] = color;
            }
          }
          FastLED.show();
          if (!delayWithStopCheck(animSpeed)) return;
        }
      }
    } 
    else if (animationType.equals("SPINNER")) {
      int segments[] = {0, 1, 8, 11, 5, 4, 9, 6};
      for (int cycle = 0; cycle < 3 && !stopFlag; cycle++) {
        for (int i = 0; i < 8 && !stopFlag; i++) {
          clearAll();
          for (int display = 0; display < DISPLAY_COUNT; display++) {
            for (int j = 0; j < LEDS_PER_SEGMENT; j++) {
              leds[segmentMap[display][segments[i]][j]] = color;
            }
          }
          FastLED.show();
          if (!delayWithStopCheck(animSpeed)) return;
        }
      }
    }
    else if (animationType.equals("FLASH")) {
      for (int flash = 0; flash < 5 && !stopFlag; flash++) {
        fill_solid(leds, NUM_LEDS, color);
        FastLED.show();
        if (!delayWithStopCheck(animSpeed)) return;
        
        clearAll();
        if (!delayWithStopCheck(animSpeed)) return;
      }
    }
    
    if (repeatCount != -1) {
      currentRepeat++;
    }
  }
  
  clearAll();
}

void loop() {
  if (commandBuffer == "STOP") {
    stopFlag = true;
    clearAll();
    commandBuffer = "";
    sendResponse("STOPPED", true);
    return;
  }
  
  String command = "";
  if (readBTCommand(command)) {
    if (command == "STOP") {
      stopFlag = true;
      clearAll();
      FastLED.show();
      commandBuffer = "";
      sendResponse("STOPPED", true);
      return;
    } 
    else {
      incomingText = command;
      
      if (incomingText.startsWith("CUSTOM|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        
        if (secondSeparator != -1) {
          String patternStr = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor;
          int displayIndex = 0;
          
          if (thirdSeparator != -1) {
            hexColor = incomingText.substring(secondSeparator + 1, thirdSeparator);
            displayIndex = incomingText.substring(thirdSeparator + 1).toInt();
            if (displayIndex < 0 || displayIndex >= DISPLAY_COUNT) displayIndex = 0;
          } else {
            hexColor = incomingText.substring(secondSeparator + 1);
          }
          
          int customPattern[16];
          for (int i = 0; i < 16 && i < patternStr.length(); i++) {
            customPattern[i] = (patternStr.charAt(i) == '1') ? 1 : 0;
          }
          
          CRGB color = parseHexColor(hexColor);
          displayCustomPattern(customPattern, color, displayIndex);
          sendResponse("CUSTOM_PATTERN_DISPLAYED", true);
        }
        else {
          sendResponse("INVALID_CUSTOM_PATTERN", false);
        }
      } 
      else if (incomingText.startsWith("SCROLL|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        int fourthSeparator = incomingText.indexOf('|', thirdSeparator + 1);
        
        if (secondSeparator != -1) {
          String text = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor;
          int scrollSpeed = 150;
          int repeatCount = 1;
          
          if (thirdSeparator != -1) {
            hexColor = incomingText.substring(secondSeparator + 1, thirdSeparator);
            
            if (fourthSeparator != -1) {
              scrollSpeed = incomingText.substring(thirdSeparator + 1, fourthSeparator).toInt();
              repeatCount = incomingText.substring(fourthSeparator + 1).toInt();
            } else {
              scrollSpeed = incomingText.substring(thirdSeparator + 1).toInt();
            }
            
            if (scrollSpeed < 50) scrollSpeed = 50;
          } else {
            hexColor = incomingText.substring(secondSeparator + 1);
          }
          
          CRGB color = parseHexColor(hexColor);
          sendResponse("SCROLLING_TEXT", true);
          scrollTextAcrossDisplays(text, color, scrollSpeed, repeatCount);
        }
        else {
          sendResponse("INVALID_SCROLL_COMMAND", false);
        }
      } 
      else if (incomingText.startsWith("RAINBOW|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        
        if (firstSeparator != -1) {
          String text = incomingText.substring(firstSeparator + 1);
          int scrollSpeed = 150;
          int repeatCount = 1;
          
          if (secondSeparator != -1) {
            text = incomingText.substring(firstSeparator + 1, secondSeparator);
            
            if (thirdSeparator != -1) {
              scrollSpeed = incomingText.substring(secondSeparator + 1, thirdSeparator).toInt();
              repeatCount = incomingText.substring(thirdSeparator + 1).toInt();
            } else {
              scrollSpeed = incomingText.substring(secondSeparator + 1).toInt();
            }
            
            if (scrollSpeed < 50) scrollSpeed = 50;
          }
          
          sendResponse("RAINBOW_TEXT", true);
          rainbowText(text, scrollSpeed, repeatCount);
        }
        else {
          sendResponse("INVALID_RAINBOW_COMMAND", false);
        }
      }
      else if (incomingText.startsWith("ANIMATE|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        int fourthSeparator = incomingText.indexOf('|', thirdSeparator + 1);
        
        if (secondSeparator != -1) {
          String animType = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor;
          int animSpeed = 100;
          int repeatCount = 1;
          
          if (thirdSeparator != -1) {
            hexColor = incomingText.substring(secondSeparator + 1, thirdSeparator);
            
            if (fourthSeparator != -1) {
              animSpeed = incomingText.substring(thirdSeparator + 1, fourthSeparator).toInt();
              repeatCount = incomingText.substring(fourthSeparator + 1).toInt();
            } else {
              animSpeed = incomingText.substring(thirdSeparator + 1).toInt();
            }
          } else {
            hexColor = incomingText.substring(secondSeparator + 1);
          }
          
          CRGB color = parseHexColor(hexColor);
          sendResponse("ANIMATION_STARTED", true);
          showAnimation(animType, color, animSpeed, repeatCount);
        }
        else {
          sendResponse("INVALID_ANIMATION_COMMAND", false);
        }
      }
      else if (command != "STOP") {
        sendResponse("UNRECOGNIZED_COMMAND: " + command, false);
      }

      incomingText = "";
    }
  }
}