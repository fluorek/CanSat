#include "GY521.h"
#include <SPI.h>
#include <SD.h>
#include <TinyGPSPlus.h>
#include <CanSatKit.h>
#include <Servo.h>

#define ss Serial //serial connection for gps



using namespace CanSatKit;


GY521 sensor(0x68);  //gy+acc
TinyGPSPlus gps; // gps
Servo myservo;  //servo
BMP280 bmp; // temp+pressure



const int chipSelect = 11; // select sd card adress

uint32_t counter = 0;

uint32_t load, stop;

static const uint32_t GPSBaud = 9600; // gps baudrate

float lastHeight = 0;

const int parachuteOpenPressure = 988;


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





  myservo.attach(9);



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

  if (!bmp.begin()) {
    // if connection failed - print message to the user
    SerialUSB.println("BMP init failed!");
    // the program will be 'halted' here and nothing will happen till restart
    while (1);
  } else {
    // print message to the user if everything is OK
    SerialUSB.println("BMP init success!");
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

  bmp.setOversampling(16);






}



void loop()
{

  double T, P;

  myservo.write(0);

  bmp.measureTemperatureAndPressure(T, P);
  //check conection to gps
  while (ss.available() > 0)
    gps.encode(ss.read());


  gy(); // gyroscope+accelerometer

  checkHeight(); //open parachute

  card(); // save to card
  // send gyroscope data via radio module
  frame.print(F(","));
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
  frame.print(F(","));

  //send gps data via radio module
  frame.print(gps.location.lat(), 6);
  frame.print(F(","));
  frame.print(gps.location.lng(), 6);
  frame.print(F(","));
  frame.print(gps.date.day());
  frame.print(F("/"));
  frame.print(gps.date.month());
  frame.print(F("/"));
  frame.print(gps.date.year());
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
  frame.print(F(","));

  //radio.transmit(frame);
  //send temperature data via radio module
  frame.print(T, 2);
  frame.print(F(","));
  frame.print(P, 2);
  radio.transmit(frame);
  frame.clear();
  delay(1000);

  delay(1000);

}

void checkHeight() {
  double T, P;
  bmp.measureTemperatureAndPressure(T, P);

  if (lastHeight > P) {
    if (P <= parachuteOpenPressure) {
      myservo.write(180);
      SerialUSB.println("OK");
      File logFile = SD.open("log.txt", FILE_WRITE);
      logFile.print("Parachute open. Pressure: ");
      logFile.println(P , 2);
    }
  }
  lastHeight = P;
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
    gpsFile.print(gps.date.day());
    gpsFile.print(F("/"));
    gpsFile.print(gps.date.month());
    gpsFile.print(F("/"));
    gpsFile.print(gps.date.year());
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
    gyFile.println();
    gyFile.close();
    // print to the SerialUSBUSB port too:

  }

  double T, P;
  bmp.measureTemperatureAndPressure(T, P);


  //  save temperature data
  File teFile = SD.open("te.txt", FILE_WRITE);
  if (teFile) {
    teFile.print(T, 2);
    teFile.print(F(","));
    teFile.print(P, 2);
    teFile.println();
    teFile.close();
  }
}
