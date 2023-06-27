/*************************************************
File:         readCOValue.ino
Description:  Receive the information automatically output by the module every second,
              and print part of the information to the serial port monitor
Note:
**************************************************/
#include <BM22S3221-1.h>

uint8_t moduleInfo[32] = {0};
uint16_t ADValue, COValue, COAlarmThreshold;

// BM22S3221_1 CO(8, &Serial); // 8->STATUS, Hardware serial
BM22S3221_1 CO(8, 2, 3); // Softeware serial:8->STATUS, 2->RX, 3->TX
void setup()
{
  CO.begin(); // Initialize Softeware serial, baud rate 9600bps, Set pin 8 as input mode

  Serial.begin(9600); // Initialize Serial, baud rate 9600bps

  Serial.println("Module preheating...(about 2 mins)");
  CO.preheatCountdown(); // Wait for the module to warm up
  Serial.println("End of module preheating.");
  Serial.println();
}
void loop()
{
  if (CO.isInfoAvailable() == true) // Scaning the serial port received buffer to receive the information sent by the module
  {
    CO.readInfoPackage(moduleInfo);
    printInfo(); // Print some information of the module
  }
}

void printInfo()
{
  /*Print CO alarm threshold (PPM)*/
  Serial.print("CO alarm threshold: ");
  COAlarmThreshold = (moduleInfo[21] << 8) + moduleInfo[22];
  Serial.print(COAlarmThreshold);
  Serial.println(" PPM");

  /*Print current CO concentration (PPM)*/
  Serial.print("CO concentration: ");
  COValue = (moduleInfo[9] << 8) + moduleInfo[10];
  Serial.print(COValue);
  Serial.println(" PPM");

  /*Print CO AD Value*/
  Serial.print("CO AD Value: ");
  ADValue = (moduleInfo[5] << 8) + moduleInfo[6];
  Serial.println(ADValue);
  Serial.println();
}
