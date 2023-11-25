#include "MS5611.h"


MS5611 MS5611(0x77);

#ifndef LED_BUILTIN
#define LED_BUILTIN    13
#endif

uint32_t start, stop;


void setup()
{
  SerialUSB.begin(9600);
  while (!SerialUSB);
  SerialUSB.println();
  SerialUSB.println(__FILE__);
  SerialUSB.print("MS5611_LIB_VERSION: ");
  SerialUSB.println(MS5611_LIB_VERSION);
  SerialUSB.println();

  pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin();
  if (MS5611.begin() == true)
  {
    SerialUSB.println("MS5611 found.");
  }
  else
  {
    SerialUSB.println("MS5611 not found. halt.");
    while (1)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
    }
  }
  SerialUSB.println();
}



void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  MS5611.setOversampling(OSR_ULTRA_LOW);
  test();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  MS5611.setOversampling(OSR_LOW);
  test();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  MS5611.setOversampling(OSR_STANDARD);
  test();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  MS5611.setOversampling(OSR_HIGH);
  test();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  MS5611.setOversampling(OSR_ULTRA_HIGH);
  test();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  SerialUSB.println();
}


void test()
{
  start = micros();
  int result = MS5611.read();
  stop = micros();
  if (result != MS5611_READ_OK)
  {
    SerialUSB.print("Error in read: ");
    SerialUSB.println(result);
  }
  else
  {
    SerialUSB.print("T:\t");
    SerialUSB.print(MS5611.getTemperature(), 2);
    SerialUSB.print("\tP:\t");
    SerialUSB.print(MS5611.getPressure(), 2);
    SerialUSB.print("\tt:\t");
    SerialUSB.print(stop - start);
    SerialUSB.println();
  }
}
