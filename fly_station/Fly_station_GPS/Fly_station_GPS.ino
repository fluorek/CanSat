

#define mySerial Serial  // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
}

void loop() { // run over and over
 
  if (mySerial.available()) {
    SerialUSB.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(SerialUSB.read());
  }
}
