## Overview

This guide provides instructions on how to connect the Fly_Station to GPS module, GY-521 (gyroscope and accelerometer), GY-63 (temperature and pressure) and a servo motor.

# Fly_Station Connection

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

## Final Presentation

- https://docs.google.com/presentation/d/1cNbn6GtGbyySS0REvJ7uqN7jcr2ff49N/edit?usp=sharing&ouid=117777603344282766644&rtpof=true&sd=true

## Reports

- https://drive.google.com/drive/folders/1zRZbbpGUpasead5YjNjED7SeBvmSdlvP?usp=sharing

## Support

- jakub.krzysik@zse.krakow.pl
