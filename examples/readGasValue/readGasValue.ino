/*************************************************
File:         readGasValue.ino
Description:  Receive the information automatically output by the module every second,
              and print part of the information to the serial port monitor
Note:
**************************************************/
#include <BM22S3021-1.h>

uint8_t moduleInfo[18] = {0};
uint8_t ADValue, gasAlarmPoint;
BM22S3021_1 gas(STATUS1, &Serial1); // Hardware serial:, STATUS1:D22
void setup()
{
  gas.begin();        // Initialize Softeware serial, baud rate 9600bps, Set pin 8 as input mode
  Serial.begin(9600); // Initialize Serial, baud rate 9600bps
  Serial.println("Module preheat...(about 3 mins)");
  gas.preheatCountdown();// Wait for the module to warm up
  Serial.println("End of module preheat.");
  Serial.println();
  delay(1200);
}
void loop()
{
  /* Scaning the serial port received buffer to receive the information sent by the module */
  if (gas.isInfoAvailable() == true)
  {
    gas.readInfoPackage(moduleInfo);
    printInfo(); // Print some information of the module
  }
}

void printInfo()
{
  /*Print gas alarm point (8-bit A/D value)*/
  Serial.print("Gas alarm point: ");
  gasAlarmPoint = moduleInfo[7];
  Serial.println(gasAlarmPoint);

  /*Print Gas A/D Value(8-bit)*/
  Serial.print("Gas A/D Value: ");
  ADValue = moduleInfo[6];
  Serial.println(ADValue);
}
