// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup(void) {
  SerialUSB.begin(9600);
  while (!SerialUSB)
    delay(10); // will pause Zero, Leonardo, etc until SerialUSB console opens

  SerialUSB.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    SerialUSB.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  SerialUSB.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  SerialUSB.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    SerialUSB.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    SerialUSB.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    SerialUSB.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    SerialUSB.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  SerialUSB.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    SerialUSB.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    SerialUSB.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    SerialUSB.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    SerialUSB.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  SerialUSB.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    SerialUSB.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    SerialUSB.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    SerialUSB.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    SerialUSB.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    SerialUSB.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    SerialUSB.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    SerialUSB.println("5 Hz");
    break;
  }

  SerialUSB.println("");
  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  SerialUSB.print("Acceleration X: ");
  SerialUSB.print(a.acceleration.x);
  SerialUSB.print(", Y: ");
  SerialUSB.print(a.acceleration.y);
  SerialUSB.print(", Z: ");
  SerialUSB.print(a.acceleration.z);
  SerialUSB.println(" m/s^2");

  SerialUSB.print("Rotation X: ");
  SerialUSB.print(g.gyro.x);
  SerialUSB.print(", Y: ");
  SerialUSB.print(g.gyro.y);
  SerialUSB.print(", Z: ");
  SerialUSB.print(g.gyro.z);
  SerialUSB.println(" rad/s");

  SerialUSB.print("Temperature: ");
  SerialUSB.print(temp.temperature);
  SerialUSB.println(" degC");

  SerialUSB.println("");
  delay(50);
}
