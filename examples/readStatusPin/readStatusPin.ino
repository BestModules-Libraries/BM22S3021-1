/*************************************************
File:         readStatusPin.ino
Description:  When the module has no alarm, the flashing period of the LED (13) is 1 second.
              When the module alarms, the flashing period of the LED (13) is 0.2 seconds
Note:
**************************************************/
#include <BM22S3221-1.h>

#define LED (13)   // LED ctrl Pin:13

// BM22S3221_1 CO(8, &Serial); // Hardware serial, 8->STATUS
BM22S3221_1 CO(8, 2, 3); // Softeware serial: 2->RX, 3->TX, 8->STATUS

void setup()
{
  CO.begin(); // Initialize Softeware serial, baud rate 9600bps, Set pin 8 as input mode

  Serial.begin(9600); // Initialize Serial, baud rate 9600bps

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println("Module preheat...(about 2 mins)");
  CO.preheatCountdown(); // Wait for the module to warm up
  Serial.println("End of module preheat.");
  Serial.println();
}

void loop()
{
  if (CO.getSTATUS() == 0)
  {
    /* No alarm */
    digitalWrite(LED, HIGH); // LED ON
    delay(500);
    digitalWrite(LED, LOW); // LED OFF
    delay(500);
  }
  if (CO.getSTATUS() == 1)
  {
    /* Alarm */
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}