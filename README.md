# Fly_Station GPS Connection

## Overview

This guide provides instructions on how to connect the Fly_Station to GPS modules (GY-521 and GY-63), a servo motor, and ESP32-CAM. Ensure proper connections to enable communication between the Fly_Station and GPS devices, as well as control the servo motor.

## Hardware Connections

### Fly_Station to GPS (GY-521 and GY-63)

- **VCC (Fly_Station) --> 5V**
- **RX (Fly_Station) --> TX5**
- **TX (Fly_Station) --> RX5**
- **GND (Fly_Station) --> GND**

### GY-521 (IMU) and GY-63 (Barometer) to Fly_Station

#### Common Connections
- **VCC --> 5V**
- **SCL --> SCL**
- **SDA --> SDA**
- **GND --> GND**

#### Additional GY-521 Connection
- **VCC (GY-521) --> 5V**
- **SCL (GY-521) --> SCL**
- **SDA (GY-521) --> SDA**
- **GND (GY-521) --> GND**

#### Additional GY-63 Connection
- **VCC (GY-63) --> 5V**
- **SCL (GY-63) --> SCL**
- **SDA (GY-63) --> SDA**
- **GND (GY-63) --> GND**

**Note:** SCL and SDA pins of GY-521 and GY-63 should be connected to the same pins on the Fly_Station.

### Servo Motor

- **Signal (Servo) --> D9**
- **Power (Servo) --> 5V**
- **Ground (Servo) --> GND**

# ESP32-CAM

To program the ESP32-CAM, select the following settings:
- **Board: AI Thinker ESP32-CAM**
- **Partition Scheme: Huge app**

## Important Notes

- Ensure all connections are secure and follow the specified voltage levels.
- Cross-verify the wiring with the datasheets of the respective modules.
- Double-check the ESP32-CAM settings to match the recommended configurations.

Following these guidelines will help establish a reliable connection between the Fly_Station, GPS modules, and the servo motor, facilitating seamless communication and control of the connected devices.
