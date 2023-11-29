#include "MS5611.h"
#include "GY521.h"
#include <SPI.h>
#include <SD.h>
#include <TinyGPSPlus.h>
#include <CanSatKit.h>

#define ss Serial //serial connection for gps

MS5611 MS5611(0x77);  // temp+pressure
GY521 sensor(0x68);  //gy+acc
TinyGPSPlus gps; // gps

const int chipSelect = 11; // select sd card adress

uint32_t counter = 0;

uint32_t load, stop;

static const uint32_t GPSBaud = 9600; // gps baudrate

using namespace CanSatKit;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz, //freqency
            SpreadingFactor_9, //factor
            CodingRate_4_8); //coding

Frame frame;

void setup()
{

  SerialUSB.begin(115200);// begin serial connection baud rate


  radio.begin(); // begin radio connection

  ss.begin(GPSBaud); // begin gps baudrate

  Wire.begin(); //begin I2C connection

  if (!SD.begin(chipSelect)) {
    SerialUSB.println("Card failed, or not present");

    while (1);
  }
  SerialUSB.println("card initialized.");


  delay(100);
  while (sensor.wakeup() == false)
  {
    SerialUSB.println("\tCould not connect to GY521");
    delay(1000);
  }
  sensor.setAccelSensitivity(0);  // 2g
  sensor.setGyroSensitivity(0);   // 250 degrees/s

  sensor.setThrottle();

  // set calibration values from calibration sketch.
  sensor.axe = 0;
  sensor.aye = 0;
  sensor.aze = 0;
  sensor.gxe = 0;
  sensor.gye = 0;
  sensor.gze = 0;

  Wire.begin();
  if (MS5611.begin() == true)
  {
    SerialUSB.println("MS5611 found.");
  }

}



void loop()
{
  //check conection to gps
  while (ss.available() > 0)
    gps.encode(ss.read());

  te();// temperature
  gy(); // gyroscope+accelerometer

  card(); // save to card
  // send gyroscope data via radio module
  frame.print(sensor.getAccelX());
  frame.print(F(","));
  frame.print(sensor.getAccelY());
  frame.print(F(","));
  frame.print(sensor.getAccelZ());
  frame.print(F(","));
  frame.print(sensor.getGyroX());
  frame.print(F(","));
  frame.print(sensor.getGyroY());
  frame.print(F(","));
  frame.print(sensor.getGyroZ());
  frame.println();
  //radio.transmit(frame);
  //send gps data via radio module
  frame.print(gps.location.lat(), 6);
  frame.print(F(","));
  frame.print(gps.location.lng(), 6);
  frame.print(F(","));
  frame.print(gps.time.hour());
  frame.print(F(":"));
  frame.print(gps.time.minute());
  frame.print(F(":"));
  frame.print(gps.time.second());
  frame.print(F(","));
  frame.print(gps.altitude.meters(), 2);
  frame.print(F(","));
  frame.print(gps.speed.kmph(), 2);
  frame.println();
  //radio.transmit(frame);
  //send temperature data via radio module
  frame.print(MS5611.getTemperature(), 2);
  frame.print(F(","));
  frame.print(MS5611.getPressure(), 2);
  frame.println();
  radio.transmit(frame);
  frame.clear();
  delay(1000);
}


void te()
{
  load = micros();
  int result = MS5611.read();
  stop = micros();
  if (result != MS5611_READ_OK)
  {
    SerialUSB.print("Error in read: ");
    SerialUSB.println(result);
  }

}

void gy() {
  sensor.read();
  int ax = sensor.getAccelX();
  int ay = sensor.getAccelY();
  int az = sensor.getAccelZ();
  int gx = sensor.getGyroX();
  int gy = sensor.getGyroY();
  int gz = sensor.getGyroZ();
  int t = sensor.getTemperature();

}
void card() {


  //save gps data
  File gpsFile = SD.open("gps.txt", FILE_WRITE);
  if (gpsFile) {
    gpsFile.print(gps.location.lat(), 6);
    gpsFile.print(F(","));
    gpsFile.print(gps.location.lng(), 6);
    gpsFile.print(F(","));
    gpsFile.print(gps.time.hour());
    gpsFile.print(F(":"));
    gpsFile.print(gps.time.minute());
    gpsFile.print(F(":"));
    gpsFile.print(gps.time.second());
    gpsFile.print(F(","));
    gpsFile.print(gps.altitude.meters(), 2);
    gpsFile.print(F(","));
    gpsFile.print(gps.speed.kmph(), 2);
    gpsFile.println();
    gpsFile.close();
  }
  //save gyroscope data
  File gyFile = SD.open("gy.txt", FILE_WRITE);
  if (gyFile) {
    gyFile.print(F(","));
    gyFile.print(sensor.getAccelX());
    gyFile.print(F(","));
    gyFile.print(sensor.getAccelY());
    gyFile.print(F(","));
    gyFile.print(sensor.getAccelZ());
    gyFile.print(F(","));
    gyFile.print(sensor.getGyroX());
    gyFile.print(F(","));
    gyFile.print(sensor.getGyroY());
    gyFile.print(F(","));
    gyFile.print(sensor.getGyroZ());
    gyFile.println ();
    gyFile.close();
    // print to the SerialUSBUSB port too:

  }
  //  save temperature data
  File teFile = SD.open("te.txt", FILE_WRITE);
  if (teFile) {
    teFile.print(MS5611.getTemperature(), 2);
    teFile.print(F(","));
    teFile.print(MS5611.getPressure(), 2);
    teFile.println();
    teFile.close();
  }
}
