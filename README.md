# WS2812 LED Alphanumeric Display

A modular and extensible alphanumeric display project using WS2812 RGB LEDs (NeoPixels). This project enables the display of scrolling text using a 16-segment representation for each character. Communication is supported via Bluetooth and Wi-Fi, with color customization and multiple display support.

## Features

- 16-segment alphanumeric character display
- Smooth scrolling text
- Bluetooth Serial and Wi-Fi communication support
- Color customization
- Multiple display chaining
- Modular codebase for easy extension

## Hardware Requirements

- ESP32 or ESP32-C3 microcontroller
- WS2812B (NeoPixel) LED strip or matrix
- Power supply capable of handling the total LED current
- Optional: HC-06 Bluetooth module (if not using onboard Bluetooth)

## Software Requirements

- Arduino IDE or PlatformIO
- FastLED library
- ESPAsyncWebServer (if using Wi-Fi)
- ESPAsyncTCP / AsyncTCP for ESP32 (if using Wi-Fi)
- BluetoothSerial (for Bluetooth mode)

