/*************************************************
File:         readGasValue.ino
Description:  Receive the information automatically output by the module every second,
              and print part of the information to the serial port monitor
Note:
**************************************************/
#include <BM22S3021-1.h>

uint8_t moduleInfo[18] = {0}, time = 180;
uint8_t ADValue, gasAlarmPoint;
uint8_t i = 1;
uint16_t tmp;
BM22S3021_1 gas(22, &Serial1); // Hardware serial, 22->STATUS
void setup()
{
  gas.begin();        // Initialize Softeware serial, baud rate 9600bps, Set pin 8 as input mode
  Serial.begin(9600); // Initialize Serial, baud rate 9600bps
  // gas.writeCommand(0xe0, 0x1b, 0x08);
  gas.restoreDefault();
  // Serial.print("preheat countdown: ");
  // while (time > 0)
  // {
  //   time = gas.readParam(0xd2, 0x51); // Get current preheat remaining time
  //   Serial.println(time);
  //   delay(1000);
  // }
  delay(1200);
}
void loop()
{
  // /* Scaning the serial port received buffer to receive the information sent by the module */
  // if (gas.isInfoAvailable() == true)
  // {
  //   gas.readInfoPackage(moduleInfo);
  //   printInfo(); // Print some information of the module
  // }
  // if (gas.readParam(0xd2, 0x50) == 0)
  // {
  //   Serial.println(i++);
  // }
  // tmp = gas.getFWVer();
  // gas.requestInfoPackage(moduleInfo);
  // gas.requestInfoPackage(moduleInfo);
  // printInfo();
  // gas.readParam(0xd2, 0x51);
  // delay(1000);
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
