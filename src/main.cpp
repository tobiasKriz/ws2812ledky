#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    64  // Doubled from 32 to support two displays
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
String commandBuffer = ""; // Add a buffer for STOP commands

// Function prototypes (updated)
void displayCharacter(char c, CRGB color, int displayIndex);
void displayText(String text, CRGB color, int displayIndex);
void displayCustomPattern(int customPattern[16], CRGB color, int displayIndex);
void scrollText(String text, CRGB color, int displayIndex, int scrollSpeed);
void scrollTextAcrossDisplays(String text, CRGB color, int scrollSpeed, int repeatCount = 1);
void rainbowText(String text, int scrollSpeed, int repeatCount = 1);
void showAnimation(String animationType, CRGB color, int animSpeed = 100, int repeatCount = 1);
CRGB getHueColor(uint8_t hue);
void stopAllAnimations();
void processEmergencyStop();

volatile bool stopFlag = false;
volatile bool emergencyStop = false; // New flag for immediate stop

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

// Enhanced delay function that's more responsive to STOP commands
bool delayWithStopCheck(int delayMs) {
  const int checkInterval = 5; // Maximum time to wait before checking for commands (milliseconds)
  unsigned long startTime = millis();
  unsigned long endTime = startTime + delayMs;
  
  // Process any pending commands in the buffer first
  if (commandBuffer == "STOP") {
    emergencyStop = true;
    stopFlag = true;
    clearAll();
    commandBuffer = ""; // Clear the buffer
    return false;
  }
  
  // Break up the delay into smaller chunks to check more frequently
  while (millis() < endTime && !stopFlag && !emergencyStop) {
    // Check for incoming commands
    if (SerialBT.available()) {
      // Process all available data
      while (SerialBT.available()) {
        char c = SerialBT.read();
        if (c == '\n') {
          // Complete command received
          commandBuffer.trim();
          if (commandBuffer == "STOP") {
            emergencyStop = true;
            stopFlag = true;
            clearAll();
            commandBuffer = ""; // Clear after processing
            return false;
          }
          commandBuffer = ""; // Reset for new commands
        } else {
          // Accumulate characters
          commandBuffer += c;
        }
      }
    }
    
    // Brief delay to allow system to process other tasks
    delay(min(checkInterval, (int)(endTime - millis())));
    yield(); // Allow the system to handle other tasks
  }
  
  return !stopFlag && !emergencyStop;
}

// Add emergency stop function that aborts all operations
void processEmergencyStop() {
  if (emergencyStop) {
    clearAll();
    stopFlag = false;
    emergencyStop = false;
    Serial.println("Emergency stop processed");
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

// Fix the scrollTextAcrossDisplays function
void scrollTextAcrossDisplays(String text, CRGB color, int scrollSpeed, int repeatCount) {
  if (text.length() == 0) return;
  stopFlag = false;
  emergencyStop = false;
  
  text.toUpperCase();
  text = "  " + text + "  ";
  
  int virtualLength = text.length();
  int currentRepeat = 0;
  
  while ((currentRepeat < repeatCount || repeatCount == -1) && !stopFlag && !emergencyStop) {
    // Check for stop command at the start of each cycle
    if (SerialBT.available()) {
      // Read character by character instead of using readStringUntil
      String command = "";
      while (SerialBT.available()) {
        char c = SerialBT.read();
        if (c == '\n') break;
        command += c;
      }
      
      command.trim(); // Trim first
      if (command == "STOP") { // Then compare
        emergencyStop = true;
        clearAll();
        break;
      }
    }
    
    for (int startChar = 0; startChar < virtualLength && !stopFlag && !emergencyStop; startChar++) {
      
      
      if (startChar < virtualLength) {
        displayCharacter(text.charAt(startChar), color, 0);
      }
      
      if (startChar + 1 < virtualLength) {
        displayCharacter(text.charAt(startChar + 1), color, 1);
      }
      
      if (!delayWithStopCheck(scrollSpeed)) {
        return; // Stop immediately if requested
      }
    }
    
    if (repeatCount != -1) {
      currentRepeat++;
    }
  }
}

 // Non-strobing rainbow text function - completely rewritten to avoid clearing the display
void rainbowText(String text, int scrollSpeed, int repeatCount) {
  if (text.length() == 0) return;
  
  // Convert to uppercase
  text.toUpperCase();
  
  // Add spaces at beginning and end for smooth scrolling
  text = "  " + text + "  ";
  
  int virtualLength = text.length();
  int currentRepeat = 0;
  
  // Reset stop flag
  stopFlag = false;
  emergencyStop = false;
  
  // Initialize hue outside the loop so it's maintained across repetitions
  uint8_t hue = 0;
  int displayDist = 64; // Color distance between displays
  int hueDelta = 2;     // How much to change the hue per update
  
  // Add support for repetition like in scrollTextAcrossDisplays
  while ((currentRepeat < repeatCount || repeatCount == -1) && !stopFlag && !emergencyStop) {
    // Process each character position
    for (int startChar = 0; startChar < virtualLength && !stopFlag && !emergencyStop; startChar++) {
      // Initial character setup (done once per position to avoid flickering)
      if (startChar < virtualLength) {
        displayCharacter(text.charAt(startChar), getHueColor(hue), 0);
      }
      
      if (startChar + 1 < virtualLength) {
        displayCharacter(text.charAt(startChar + 1), getHueColor(hue + displayDist), 1);
      }
      
      // Instead of multiple refreshes with clearing, stay on this character
      // and just update the color in place for a smoother transition
      int colorUpdates = scrollSpeed / 15;  // Number of color updates while showing the same character
      
      for (int i = 0; i < colorUpdates && !stopFlag && !emergencyStop; i++) {
        // Update color of existing LEDs without redrawing the whole character
        int currentIndex;
        if (text.charAt(startChar) == ' ') currentIndex = 36;
        else if (text.charAt(startChar) >= '0' && text.charAt(startChar) <= '9') currentIndex = text.charAt(startChar) - '0';
        else if (text.charAt(startChar) >= 'A' && text.charAt(startChar) <= 'Z') currentIndex = text.charAt(startChar) - 'A' + 10;
        
        // Update first display with new hue
        CRGB color1 = getHueColor(hue);
        for (int seg = 0; seg < SEGMENT_COUNT; seg++) {
          if (numbers[currentIndex][seg]) {
            for (int j = 0; j < LEDS_PER_SEGMENT; j++) {
              leds[segmentMap[0][seg][j]] = color1;
            }
          }
        }
        
        // Update second display with new hue (if showing a character)
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
        hue += hueDelta; // Increment hue for color cycling
        
        if (!delayWithStopCheck(15)) { // Short delay for color updates
          return;
        }
      }
    }
    
    // Increment repeat counter if we're not in endless mode
    if (repeatCount != -1) {
      currentRepeat++;
    }
  }
}

// Function to get RGB color from hue value
CRGB getHueColor(uint8_t hue) {
  // Convert hue to RGB using FastLED's HSV to RGB conversion
  CHSV hsv(hue, 240, 255);
  CRGB rgb;
  hsv2rgb_rainbow(hsv, rgb);
  return rgb;
}

// Modified animation function to use the repeat count
void showAnimation(String animationType, CRGB color, int animSpeed, int repeatCount) {
  // Reset stop flag
  stopFlag = false;
  emergencyStop = false;
  
  // Default to 1 repeat if none specified or invalid
  if (repeatCount < -1) repeatCount = 1;
  int currentRepeat = 0;
  
  // Loop through repeats
  while ((currentRepeat < repeatCount || repeatCount == -1) && !stopFlag && !emergencyStop) {
    if (animationType.equals("WAVE")) {
      // Wave animation - segments light up in sequence
      for (int cycle = 0; cycle < 3 && !stopFlag && !emergencyStop; cycle++) {
        for (int segment = 0; segment < SEGMENT_COUNT && !stopFlag && !emergencyStop; segment++) {
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
      // Spinning effect - rotating segment around the display
      int segments[] = {0, 1, 8, 11, 5, 4, 9, 6}; // Segments to light in sequence
      for (int cycle = 0; cycle < 3 && !stopFlag && !emergencyStop; cycle++) { // Reduced from 5 cycles to 3
        for (int i = 0; i < 8 && !stopFlag && !emergencyStop; i++) {
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
      // Flash animation - all LEDs flash together
      for (int flash = 0; flash < 5 && !stopFlag && !emergencyStop; flash++) {
        fill_solid(leds, NUM_LEDS, color);
        FastLED.show();
        if (!delayWithStopCheck(animSpeed)) return;
        
        clearAll();
        if (!delayWithStopCheck(animSpeed)) return;
      }
    }
    
    // Increment repeat counter if we're not in endless mode
    if (repeatCount != -1) {
      currentRepeat++;
    }
  }
  
  clearAll();
}

// Pre-defined animations
void showAnimation(String animationType, CRGB color, int animSpeed) {
  // Reset stop flag
  stopFlag = false;
  emergencyStop = false;
  
  if (animationType.equals("WAVE")) {
    // Wave animation - segments light up in sequence
    for (int cycle = 0; cycle < 3 && !stopFlag && !emergencyStop; cycle++) {
      for (int segment = 0; segment < SEGMENT_COUNT && !stopFlag && !emergencyStop; segment++) {
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
    // Spinning effect - rotating segment around the display
    int segments[] = {0, 1, 8, 11, 5, 4, 9, 6}; // Segments to light in sequence
    for (int cycle = 0; cycle < 5 && !stopFlag && !emergencyStop; cycle++) {
      for (int i = 0; i < 8 && !stopFlag && !emergencyStop; i++) {
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
    // Flash animation - all LEDs flash together
    for (int flash = 0; flash < 5 && !stopFlag && !emergencyStop; flash++) {
      fill_solid(leds, NUM_LEDS, color);
      FastLED.show();
      if (!delayWithStopCheck(animSpeed)) return;
      
      clearAll();
      if (!delayWithStopCheck(animSpeed)) return;
    }
  }
  
  clearAll();
}

// Make the stopAllAnimations function more forceful
void stopAllAnimations() {
  stopFlag = true;
  emergencyStop = true;
  delay(10); // Brief delay to let things settle
  clearAll();
}

// Modified loop function to more aggressively check for STOP commands
void loop() {
  // Process emergency stop signals
  processEmergencyStop();
  
  // Check for STOP command in buffer before processing other commands
  if (commandBuffer == "STOP") {
    Serial.println("STOP command in buffer processed");
    emergencyStop = true;
    stopFlag = true;
    clearAll();
    commandBuffer = "";
  }
  
  // Process incoming serial data
  while (SerialBT.available()) {
    char c = SerialBT.read();
    
    // If we receive a newline, process the complete command
    if (c == '\n') {
      incomingText.trim();
      
      // Priority handling for STOP command
      if (incomingText == "STOP") {
        Serial.println("STOP command received");
        // Stop everything immediately - more aggressive action
        emergencyStop = true;
        stopFlag = true;
        clearAll();
        FastLED.show(); // Force immediate update of LEDs
        incomingText = "";
        commandBuffer = ""; // Clear any buffered commands
        continue;
      }
      
      // Regular command processing
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
      // Check if this is a scrolling text command with repetition
      else if (incomingText.startsWith("SCROLL|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        int fourthSeparator = incomingText.indexOf('|', thirdSeparator + 1);
        
        if (secondSeparator != -1) {
          String text = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor;
          int scrollSpeed = 150; // Default speed
          int repeatCount = 1;   // Default repeat count (1 = once)
          
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
          Serial.println("Scrolling text across displays: " + text + " | #" + hexColor + 
                         " with speed " + String(scrollSpeed) + 
                         " repeats " + (repeatCount == -1 ? "endless" : String(repeatCount)));
          scrollTextAcrossDisplays(text, color, scrollSpeed, repeatCount);
        }
      } 
      // Rainbow text effect with repetition
      else if (incomingText.startsWith("RAINBOW|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        
        if (firstSeparator != -1) {
          String text = incomingText.substring(firstSeparator + 1);
          int scrollSpeed = 150; // Default speed
          int repeatCount = 1;   // Default repeat count
          
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
          
          Serial.println("Rainbow text: " + text + " with speed " + String(scrollSpeed) + 
                         " repeats " + (repeatCount == -1 ? "endless" : String(repeatCount)));
          rainbowText(text, scrollSpeed, repeatCount);
        }
      }
      
      // Pre-defined animations
      else if (incomingText.startsWith("ANIMATE|")) {
        int firstSeparator = incomingText.indexOf('|');
        int secondSeparator = incomingText.indexOf('|', firstSeparator + 1);
        int thirdSeparator = incomingText.indexOf('|', secondSeparator + 1);
        int fourthSeparator = incomingText.indexOf('|', thirdSeparator + 1);
        
        if (secondSeparator != -1) {
          String animType = incomingText.substring(firstSeparator + 1, secondSeparator);
          String hexColor;
          int animSpeed = 100; // Default speed
          int repeatCount = 1; // Default repeat count
          
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
          Serial.println("Animation: " + animType + " with speed " + String(animSpeed) + 
                        " repeats " + (repeatCount == -1 ? "endless" : String(repeatCount)));
          showAnimation(animType, color, animSpeed, repeatCount);
        }
      }

      incomingText = "";
    } else {
      incomingText += c;
      
      // Special case: detect "STOP" even if not complete line
      if (incomingText.endsWith("STOP")) {
        Serial.println("Potential STOP detected");
        commandBuffer = "STOP"; // Mark for potential processing
      }
    }
  }
  
  // Always check for STOP commands periodically even inside other operations
  if (commandBuffer == "STOP") {
    Serial.println("Buffered STOP command processed");
    emergencyStop = true;
    stopFlag = true;
    clearAll();
    commandBuffer = "";
  }
}
