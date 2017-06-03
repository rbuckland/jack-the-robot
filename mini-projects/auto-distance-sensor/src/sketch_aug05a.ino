
#include <Wire.h>
#include <Servo.h>

Servo hinge;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  hinge.attach(9);
}

uint16_t readings[4] = {0};

#define MIN_RANGE 350
#define ARTICULATION 120
#define MAX_RANGE 1200
#define ZERO 0

int reading = 0;

void loop() {
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(112); // transmit to device #112 (0x70)
  // the address specified in the datasheet is 224 (0xE0)
  // but i2c adressing uses the high 7 bits so it's 112
  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)
  Wire.write(byte(0x52));      // command sensor to measure in "inches" (0x50)
  // use 0x51 for centimeters
  // use 0x52 for ping microseconds
  Wire.endTransmission();      // stop transmitting

  // step 2: wait for readings to happen
  delay(70);                   // datasheet suggests at least 65 milliseconds

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
    readings[4] = reading;
    Serial.println(reading);
  }



  if(readings[4] > MAX_RANGE || readings[4] < MIN_RANGE) {
    return;
  }

  for (int i = 0; i < 4; i++)
    readings[i] = readings[i + 1];
  
  
  int smoothed = (3 * readings[0] + 12 * readings[1] + 17 * readings[2] + 12 * readings[3] + 3 * readings[4]) / 35;
  
  
  Serial.println(map(smoothed, MIN_RANGE, MAX_RANGE, ZERO, ZERO + ARTICULATION));

  hinge.write(map(smoothed, MIN_RANGE, MAX_RANGE, ZERO, ZERO + ARTICULATION));

}

