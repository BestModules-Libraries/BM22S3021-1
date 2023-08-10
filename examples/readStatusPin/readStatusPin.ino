/*************************************************
File:         readStatusPin.ino
Description:  When the module has no alarm, the flashing period of the LED (13) is 1 second.
              When the module alarms, the flashing period of the LED (13) is 0.2 seconds
Note:
**************************************************/
#include <BM22S3021-1.h>

#define LED (13) // LED ctrl Pin:13

BM22S3021_1 gas(STATUS1, &Serial1); // Hardware serial:, STATUS1:D22

void setup()
{
  gas.begin(); // Initialize Softeware serial, baud rate 9600bps, Set pin 8 as input mode

  Serial.begin(9600); // Initialize Serial, baud rate 9600bps

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println("Module preheat...(about 3 mins)");
  gas.preheatCountdown(); // Wait for the module to warm up
  Serial.println("End of module preheat.");
  Serial.println();
}

void loop()
{
  if (gas.getSTATUS() == 0)
  {
    /* No alarm */
    digitalWrite(LED, HIGH); // LED ON
    delay(500);
    digitalWrite(LED, LOW); // LED OFF
    delay(500);
  }
  if (gas.getSTATUS() == 1)
  {
    /* Alarm */
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}
