# WS2812 LED Alphanumeric Display

A modular and extensible alphanumeric display project using WS2812 RGB LEDs (NeoPixels). This project enables the display of scrolling text using a 16-segment representation for each character. It communicates exclusively over Bluetooth and is designed to run on a custom PCB using an ESP32 or ESP32-C3.

## Features

- 16-segment alphanumeric character display
- Smooth scrolling text
- Bluetooth Serial communication
- Color customization
- Support for multiple daisy-chained displays
- Modular codebase for easy customization and maintenance
- Web-based control interface

## Project Website

You can use the project with a web-based interface for sending text and color data via Bluetooth:

[https://tobiaskriz.github.io/ws2812ledky](https://tobiaskriz.github.io/ws2812ledky)

This page uses the Web Serial API and works best in Chromium-based browsers (like Chrome or Edge). Make sure your ESP32 is in Bluetooth mode and paired with your computer.

## Hardware Requirements

- Custom PCB with:
  - ESP32 or ESP32-C3 microcontroller
  - WS2812B (NeoPixel) LED array

## Software Requirements

- Arduino IDE or PlatformIO
- [FastLED](https://github.com/FastLED/FastLED) library
- [BluetoothSerial](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/bluetooth/bluetoothserial.html) (built into ESP32 Arduino core)

