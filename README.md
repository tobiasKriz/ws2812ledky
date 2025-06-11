# WS2812 LED Alphanumeric Display

A modular and extensible alphanumeric display project using WS2812 RGB LEDs (NeoPixels). This project enables the display of scrolling text using a 16-segment representation for each character. It communicates over Bluetooth using an HC-06 module and is designed to run on a custom PCB using an ESP32 or ESP32-C3.

## Features

- 16-segment alphanumeric character display
- Smooth scrolling text
- Bluetooth Serial communication via HC-06
- Color customization
- Support for multiple daisy-chained displays
- Modular codebase for easy customization and maintenance
- Web-based control interface

## Project Website

You can use the project with a web-based interface for sending text and color data via Bluetooth:

[https://tobiaskriz.github.io/ws2812ledky](https://tobiaskriz.github.io/ws2812ledky)

This page uses the Web Serial API and works best in Chromium-based browsers (like Chrome or Edge). Make sure your HC-06 is paired with your computer and assigned to a COM port.

## Hardware Requirements

- Custom PCB with:
  - ESP32 or ESP32-C3 microcontroller
  - WS2812B (NeoPixel) LED array
  - HC-06 Bluetooth module connected to a UART port (typically TX/RX)

## Software Requirements

- Arduino IDE or PlatformIO
- [FastLED](https://github.com/FastLED/FastLED) library
- SoftwareSerial

