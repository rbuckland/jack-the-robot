/*
Generic example for the SRF modules 02, 08, 10 and 235.
Only the SRF08 uses the light saensor so when any other
range finder is used with this code the light reading will
be a constant value.
*/

#include <Wire.h>
#include <SoftwareSerial.h>

#define LCD_RX              0x02                                   // Software serial pin for rx
#define LCD_TX              0x03                                   // Software serial pin for tx
#define SRF_ADDRESS         0x70                                   // Address of the SRF08
#define CMD                 (byte)0x00                             // Command byte, values of 0 being sent with write have to be masked as a byte to stop them being misinterpreted as NULL this is a bug with arduino 1.0
#define RANGEBYTE           0x02                                   // Byte for start of ranging data

byte highByte = 0x00;                             // Stores high byte from ranging
byte lowByte = 0x00;                              // Stored low byte from ranging

void setup(){

  Serial.begin(9600);                             // Begins serial port for Serial
  Wire.begin();
  delay(100);                                     // Waits to make sure everything is powered up before sending or receiving data

  Serial.println("SRF02/08/10/235");

  int softRev = getSoft();                        // Calls function to get software revision
  Serial.print("Software version: ");
  Serial.print(softRev, DEC);                     // Print softRev to LCD03

}

void loop(){

  int rangeData = getRange();                     // Calls a function to get range
  Serial.print("Range = ");
  Serial.println(rangeData, DEC);                   // Print rangeData to LCD03                      // Print some spaces to slear spaces after data

  delay(100);                                      // Wait before looping
}

int getRange(){                                   // This function gets a ranging from the SRF08

  int range = 0;

  Wire.beginTransmission(SRF_ADDRESS);             // Start communticating with SRF08
  Wire.write(CMD);                                 // Send Command Byte
  Wire.write(0x52);                                // Send 0x51 to start a ranging (cm's) - 0x52 microseconds
  Wire.endTransmission();

  delay(100);                                      // Wait for ranging to be complete

  Wire.beginTransmission(SRF_ADDRESS);             // start communicating with SRFmodule
  Wire.write(RANGEBYTE);                           // Call the register for start of ranging data
  Wire.endTransmission();

  Wire.requestFrom(SRF_ADDRESS, 2);                // Request 2 bytes from SRF module
  while(Wire.available() < 2);                     // Wait for data to arrive
  highByte = Wire.read();                          // Get high byte
  lowByte = Wire.read();                           // Get low byte

  range = (highByte << 8) + lowByte;               // Put them together

  return(range);                                   // Returns Range
}


int getSoft(){                                     // Function to get software revision

  Wire.beginTransmission(SRF_ADDRESS);             // Begin communication with the SRF module
  Wire.write(CMD);                                 // Sends the command bit, when this bit is read it returns the software revision
  Wire.endTransmission();

  Wire.requestFrom(SRF_ADDRESS, 1);                // Request 1 byte
  while(Wire.available() < 0);                     // While byte available
  int software = Wire.read();                      // Get byte

  return(software);

}
