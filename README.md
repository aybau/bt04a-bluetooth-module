# BT04A Bluetooth Module

Arduino C++ and Python libraries for the BT04A Bluetooth module (based on the CC2541 chip).

## Functional characteristics

### Arduino Library Features
- Supports both hardware serial and software serial ports
- AT Command Configuration
- BLE Data Transmission
- Multiple working modes switching
- Event callback mechanism
- Automatic Baud Rate Detection
- Connection status management

### Features of Python Libraries
- Cross-platform support (Windows/Linux/macOS)
- Automatic port detection
- Multithreaded data reception
- Asynchronous event handling
- Full AT command set support
- Context manager support

## Installation

### Installing Arduino Libraries
1. Copy the `BT04A` folder to the `libraries` directory of Arduino
2. Restart the Arduino IDE
3. Open the example: File → Examples → BT04A → BasicExample

### Installing Python Libraries
```bash
# 从源码安装
pip install .

# 或直接使用
pip install bt04a
```

## Compatibility

| Development Board | Normal Operation | Operation Failure | Untested | Notes |
| ------------ | :--: | :----: | :----: | :--: |
| Arduino UNO | √ | | | |
| Arduino Mega | √ | | | |
| Arduino Leonardo | √ | | | |
| ESP8266 | √ | | | Large Memory Support |
| micro:bit | √ | | | Large memory support |

## Historical Versions
Release of V1.0 on February 9, 2026

## Author
Written by aybau,2026.