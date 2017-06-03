
#include <Wire.h>
#include <Servo.h>

Servo hinge;

#define MIN_RANGE 350
#define MAX_RANGE 1200
#define ARTICULATION 120
#define ZERO 0

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

void sensor_setup() {
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(112); // transmit to device #112 (0x70)
  // the address specified in the datasheet is 224 (0xE0)
  // but i2c adressing uses the high 7 bits so it's 112
  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)
  Wire.write(byte(0x52));      // command sensor to measure in "inches" (0x50)
  // use 0x51 for centimeters
  // use 0x52 for ping microseconds
  Wire.endTransmission();      // stop transmitting
}

int sensor_read() {


  sensor_setup();

  delay(70);                   // datasheet suggests at least 65 milliseconds

  int reading = 0;

  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(112); // transmit to device #112
  Wire.write(byte(0x02));      // sets register pointer to echo #1 register (0x02)
  Wire.endTransmission();      // stop transmitting

  // step 4: request reading from sensor
  Wire.requestFrom(112, 2);    // request 2 bytes from slave device #112

  // step 5: receive reading from sensor
  if (2 <= Wire.available()) { // if two bytes were received
    reading = Wire.read();  // receive high byte (overwrites previous reading)
    reading = reading << 8;    // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    Serial.println(reading);
    return reading;
  } else {
    Serial.println("no reading received");
    return -1;
  }
}

void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  Wire.begin();
  hinge.attach(9);

  // initialize all the readings to 0:
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

}

void loop() {

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = sensor_read();
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits

  Serial.println(map(average, MIN_RANGE, MAX_RANGE, ZERO, ZERO + ARTICULATION));
  hinge.write(map(average, MIN_RANGE, MAX_RANGE, ZERO, ZERO + ARTICULATION));

  // delay in between reads for stability
  delay(1); 

}
