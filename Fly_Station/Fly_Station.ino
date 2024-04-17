// Include necessary libraries
#include "GY521.h"
#include <SPI.h>
#include <SD.h>
#include <TinyGPSPlus.h>
#include <CanSatKit.h>
#include <Servo.h>

#define ss Serial // Define ss as Serial

using namespace CanSatKit; // Use the CanSatKit namespace


GY521 sensor(0x68); // Initialize GY521 sensor with I2C address 0x68
TinyGPSPlus gps; // Initialize TinyGPSPlus object for GPS
Servo myservo;  // Initialize Servo 
BMP280 bmp; // Initialize BMP280 pressure and temperature sensor
Frame frame; // Initialize an object of type Frame

const int chipSelect = 11; // Set the chip select pin for the SD card as 11
const uint32_t GPSBaud = 9600; // Set the GPS module baud rate to 9600
const int servoPin = 9; // Set servo to pin 9 (D9)

bool started = false;
long elapsedTime = 0;
long calibrateTime = 0;

// Initialize Radio object
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.8,
            Bandwidth_125000_Hz, // Freqency
            SpreadingFactor_9, // Factor
            CodingRate_4_8); // Coding

void setup() {
  SerialUSB.begin(115200); // Begin serial communication with a baud rate of 115200

  myservo.attach(servoPin); // Attache the servo motor to the digital pin

  radio.begin(); // Begin radio connection

  ss.begin(GPSBaud); // Begin gps baudrate

  Wire.begin(); // Begin I2C connection

  // Attempt to initialize the SD card
  if (!SD.begin(chipSelect)) {
    SerialUSB.println("Card failed, or not present"); // Print an error message if SD card initialization fails
    while (1);
  }

  // Attempt to initialize the gyroscope and accelerometer sensor
  while (sensor.wakeup() == false){
    SerialUSB.println("Could not connect to GY521"); // Print an error message if gyroscope initialization fails
    delay(1000);
  }

  // Attempt to initialize the temperature and pressure sensor
  if (!bmp.begin()) {
    SerialUSB.println("BMP init failed!"); // Print an error message if BMP sensor initialization fails
    while (1);
  }

  sensor.setAccelSensitivity(0);  // Set accelerometer sensitivity to 2g
  sensor.setGyroSensitivity(0);   // Set gyroscope sensitivity to 250 degrees/s

  sensor.setThrottle(); // Set sensor throttle (adjust the data output rate)

  // Set calibration values from calibration sketch.
  sensor.axe = 0;
  sensor.aye = 0;
  sensor.aze = 0;
  sensor.gxe = 0;
  sensor.gye = 0;
  sensor.gze = 0;

  bmp.setOversampling(16); // Set oversampling for BMP sensor to 16x
  
  // Save height to file to prevent miss parachute opening
  if (SD.exists("save.txt")) {
    // Read calibrateTime from save.txt
    File saveFile = SD.open("save.txt", FILE_READ);

    if (saveFile) {
      String line1 = saveFile.readStringUntil(',');
      started = line1.toInt() == 1 ? true : false;
      String line2 = saveFile.readStringUntil('\n');
      elapsedTime = line2.toInt();
      saveFile.close();
    }
  }

  myservo.write(180); // Reset servo position
  
  SerialUSB.println("Program started"); // Print a message indicating that the program has started

  // Save a message indicating that the program has started to log.txt
  File logFile = SD.open("log.txt", FILE_WRITE);
  logFile.println("Program started. Time: ");
  logFile.print(gps.time.hour());
  logFile.print(F(":"));
  logFile.print(gps.time.minute());
  logFile.print(F(":"));
  logFile.println(gps.time.second());
  logFile.close();
}



void loop() {

  // Read temperature and pressure data
  double T, P;
  bmp.measureTemperatureAndPressure(T, P);
  
  // Check GPS conection
  while (ss.available() > 0)
    gps.encode(ss.read());

  // Read gyroscope and accelerometer data
  sensor.read();
  int ax = sensor.getAccelX();
  int ay = sensor.getAccelY();
  int az = sensor.getAccelZ();
  int gx = sensor.getGyroX();
  int gy = sensor.getGyroY();
  int gz = sensor.getGyroZ();
  int t = sensor.getTemperature();

  parachuteCheck(); // Checking whether to deploy the parachute

  saveToSDCard(); // Save data to SD card
  
  // Send gyroscope and accelerometer data via radio module
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

  // Send GPS data via radio module
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
  
  // Send temperature and pressure data via radio module
  frame.print(T, 2);
  frame.print(F(","));
  frame.print(P, 2);
  radio.transmit(frame);
  frame.clear();

  // Wait 1 sec
  delay(1000);
}

void parachuteCheck() {
  File logFile = SD.open("log.txt", FILE_WRITE);
  
  SerialUSB.println(analogRead(A0));
  SerialUSB.println(millis());
  SerialUSB.println(started);
  SerialUSB.println(elapsedTime);
  SerialUSB.println(calibrateTime);
  SerialUSB.println(millis() - calibrateTime + elapsedTime);
  
  if(analogRead(A0) >= 600 && millis() >= 120000 && !started){ 
    started = true;
  }

  if(started){
    frame.print("True");
  }else{
    frame.print("False");
  }

  if(started && analogRead(A0) <= 500 && calibrateTime == 0){
    calibrateTime = millis();
  }
  
  if (millis() - calibrateTime + elapsedTime >= 20000 && calibrateTime != 0 && started) {
    myservo.write(0); // Set servo position to 180 degrees to open the parachute

    // Save a message indicating that the parachute is open to log.txt
    logFile.print("Secure parachute open. Time: ");
    logFile.print(gps.time.hour());
    logFile.print(F(":"));
    logFile.print(gps.time.minute());
    logFile.print(F(":"));
    logFile.println(gps.time.second());

    frame.print(F(","));
    frame.print("True");
  }else{
    frame.print(F(","));
    frame.print("False");
  }



  SD.remove("save.txt");
  File saveFile = SD.open("save.txt", FILE_WRITE);
  if (saveFile) {
    saveFile.println(started ? 1 : 0);
    saveFile.print(",");
    if(calibrateTime != 0 && started){
      saveFile.print(millis() - calibrateTime + elapsedTime);
    }else{
      saveFile.print("0");
    }
    saveFile.close();
  }

  
  logFile.close();
}

void saveToSDCard() {
  // Save data to SD card
  
  // Save GPS data to gps.txt
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
  
  // Save gyroscope and accelerometer data to gy.txt
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
  }

  // Save temperature and pressure data to te.txt
  double T, P;
  bmp.measureTemperatureAndPressure(T, P);

  File teFile = SD.open("te.txt", FILE_WRITE);
  if (teFile) {
    teFile.print(T, 2);
    teFile.print(F(","));
    teFile.print(P, 2);
    teFile.println();
    teFile.close();
  }
}
