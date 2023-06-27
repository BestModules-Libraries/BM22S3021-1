/*****************************************************************
File:        BM22S3221-1.cpp
Author:      BESTMODULES
Description: None
History:     None
V1.0.1  --Initial version; 2023-02-14; Arduino IDE : v1.8.19
******************************************************************/
#include "BM22S3221-1.h"

/**********************************************************
Description: Constructor
Parameters: statusPin：Status pin connection with Arduino
            *theSerial : Serial object if your board has more than one UART interface
Return: None
Others: None
**********************************************************/
BM22S3221_1::BM22S3221_1(uint8_t statusPin, HardwareSerial *theSerial)
{
  _softSerial = NULL;
  _statusPin = statusPin;
  _hardSerial = theSerial;
}

/**********************************************************
Description:  Constructor
Parameters: statusPin：Status pin connection with Arduino
            rxPin : Receiver pin of the UART
            txPin : Send signal pin of UART
Return: None
Others: None
**********************************************************/
BM22S3221_1::BM22S3221_1(uint8_t statusPin, uint8_t rxPin, uint8_t txPin)
{
  _hardSerial = NULL;
  _statusPin = statusPin;
  _rxPin = rxPin;
  _txPin = txPin;
  _softSerial = new SoftwareSerial(_rxPin, _txPin);
}

/**********************************************************
Description:  Module initialization
Parameters: None
Return: None
Others: None
**********************************************************/
void BM22S3221_1::begin()
{
  if (_softSerial != NULL)
  {
    _softSerial->begin(BAUDRATE); // baud rate:9600
  }
  if (_hardSerial != NULL)
  {
    _hardSerial->begin(BAUDRATE); // baud rate:9600
  }
  pinMode(_statusPin, INPUT);
}

/**********************************************************
Description: Wait for the module warm-up to complete(about 120 seconds)
Parameters: None
Return: None
Others: None
**********************************************************/
void BM22S3221_1::preheatCountdown()
{
  countdown(_preheat);
}

/**********************************************************
Description: Get STATUS pin level
Parameters: None
Return: None
Others: None
**********************************************************/
uint8_t BM22S3221_1::getSTATUS()
{
  return digitalRead(_statusPin);
}

/**********************************************************
Description: Get the current status of the module
Parameters: None
Return: module status (refer to datasheet for meaning of each bit)
Others: None
**********************************************************/
uint8_t BM22S3221_1::getWorkStatus()
{
  uint8_t tmp = 0;
  uint8_t sendBuf[4] = {0xD2, 0x88, 0x00, 0xA6};
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
  }
  return tmp;
}

/**********************************************************
Description: Read CO concentration value
Parameters: None
Return: CO concentration value in ppm
Others: None
**********************************************************/
uint16_t BM22S3221_1::readCOValue()
{
  uint16_t tmp = 0;
  uint8_t sendBuf1[4] = {0xD2, 0x84, 0x00, 0xAA};
  uint8_t sendBuf0[4] = {0xD2, 0x85, 0x00, 0xA9};
  uint8_t recBuf[8];
  writeBytes(sendBuf1, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
    tmp = tmp << 8;
    writeBytes(sendBuf0, 4);
    delay(20); // Wait for the module to reply data
    if (readBytes(recBuf, 8) == 0)
    {
      tmp += recBuf[6];
    }
    else
    {
      tmp = 0;
    }
  }
  return tmp;
}

/**********************************************************
Description: Read CO AD value
Parameters: None
Return: AD value for CO concentration
Others: None
**********************************************************/
uint16_t BM22S3221_1::readADValue()
{
  uint16_t tmp = 0;
  uint8_t sendBuf1[4] = {0xD2, 0x80, 0x00, 0xAE};
  uint8_t sendBuf0[4] = {0xD2, 0x81, 0x00, 0xAD};
  uint8_t recBuf[8];
  writeBytes(sendBuf1, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
    tmp = tmp << 8;
    writeBytes(sendBuf0, 4);
    delay(20); // Wait for the module to reply data
    if (readBytes(recBuf, 8) == 0)
    {
      tmp += recBuf[6];
    }
    else
    {
      tmp = 0;
    }
  }
  return tmp;
}

/**********************************************************
Description: Read power-on reference value
Parameters: None
Return: power-on reference value
Others: None
**********************************************************/
uint16_t BM22S3221_1::readRefValue()
{
  uint16_t tmp = 0;
  uint8_t sendBuf1[4] = {0xD2, 0x82, 0x00, 0xAC};
  uint8_t sendBuf0[4] = {0xD2, 0x83, 0x00, 0xAB};
  uint8_t recBuf[8];
  writeBytes(sendBuf1, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
    tmp = tmp << 8;
    writeBytes(sendBuf0, 4);
    delay(20); // Wait for the module to reply data
    if (readBytes(recBuf, 8) == 0)
    {
      tmp += recBuf[6];
    }
    else
    {
      tmp = 0;
    }
  }
  return tmp;
}

/**********************************************************
Description: Get the current status and data of the module
Parameters: array: The array for storing the 32-byte module information
                  (refer to datasheet for meaning of each bit)
Return: 0: check ok
        1: check error
        2: timeout error
Others: None
**********************************************************/
uint8_t BM22S3221_1::requestInfoPackage(uint8_t array[])
{
  uint8_t errFlag = 1;
  uint8_t sendBuf[4] = {0xAC, 0x00, 0x00, 0x54};
  uint8_t recBuf[32];
  writeBytes(sendBuf, 4);
  delay(40); // Wait for the module to reply data
  errFlag = readBytes(recBuf, 32);
  if (errFlag == 0)
  {
    for (uint8_t i = 0; i < 32; i++)
    {
      array[i] = recBuf[i];
    }
  }
  return errFlag;
}

/**********************************************************
Description: Query whether the 32-byte data sent by the module is received
Parameters: None
Return: true(1): 32-byte data received
        false(0): 32-byte data not received
Others: Only used in the mode of Tx Auto Output Info
**********************************************************/
bool BM22S3221_1::isInfoAvailable()
{
  uint8_t header[5] = {0xAA, 0x20, 0x21, 0x01, 0xAC}; // Fixed code for first 5 bytes of 32-byte data
  uint8_t recBuf[32] = {0}, recLen = 32;
  uint8_t i, num = 0, readCnt = 0, failCnt = 0, checkCode = 0;
  bool isHeader = false, result = false;

  /* Select hardSerial or softSerial according to the setting */
  if (_softSerial != NULL)
  {
    num = _softSerial->available();
  }
  else if (_hardSerial != NULL)
  {
    num = _hardSerial->available();
  }

  /* Serial buffer contains at least one 32-byte data */
  if (num >= recLen)
  {
    while (failCnt < 2) // Didn't read the required data twice, exiting the loop
    {
      /* Find 5-byte data header */
      for (i = 0; i < 5;)
      {
        if (_softSerial != NULL)
        {
          recBuf[i] = _softSerial->read();
        }
        else if (_hardSerial != NULL)
        {
          recBuf[i] = _hardSerial->read();
        }

        if (recBuf[i] == header[i])
        {
          isHeader = true; // Fixed code is correct
          i++;             // Next byte
        }
        else if (recBuf[i] != header[i] && i > 0)
        {
          isHeader = false; // Next fixed code error
          failCnt++;
          break;
        }
        else if (recBuf[i] != header[i] && i == 0)
        {
          readCnt++; // 0xAA not found, continue
        }
        if (readCnt > (num - recLen))
        {
          return false; // No data found
        }
      }

      /* Find the correct fixed code */
      if (isHeader)
      {
        for (i = 0; i < 5; i++)
        {
          checkCode += recBuf[i]; // Sum checkCode
        }
        for (i = 5; i < recLen; i++) // Read subsequent 27-byte data
        {
          if (_softSerial != NULL)
          {
            recBuf[i] = _softSerial->read();
          }
          else if (_hardSerial != NULL)
          {
            recBuf[i] = _hardSerial->read();
          }
          checkCode += recBuf[i]; // Sum checkCode
        }
        checkCode = checkCode - recBuf[recLen - 1];
        checkCode = (~checkCode) + 1; // Calculate checkCode

        /* Compare whether the check code is correct */
        if (checkCode == recBuf[recLen - 1])
        {
          for (i = 0; i < recLen; i++)
          {
            _recBuf[i] = recBuf[i]; // True, assign data to _recBuf[]
          }
          result = true;
          break; // Exit "while (failCnt < 2)" loop
        }
        else
        {
          failCnt++; // Error, failCnt plus 1, return "while (failCnt < 2)" loop
          checkCode = 0;
        }
      }
    }
  }
  return result;
}

/**********************************************************
Description: Read the 32-byte data of sent by the module
Parameters: array: The array for storing the 32-byte module information
                  (refer to datasheet for meaning of each bit)
Return: None
Others: Use after isInfoAvailable()
**********************************************************/
void BM22S3221_1::readInfoPackage(uint8_t array[])
{
  for (uint8_t i = 0; i < 32; i++)
  {
    array[i] = _recBuf[i];
  }
}

/**********************************************************
Description: reset module
Parameters: None
Return: 0: check ok
        1: check error
        2: timeout error
Others: After the command is executed, the module needs to be preheated again
**********************************************************/
uint8_t BM22S3221_1::resetModule()
{
  uint8_t sendBuf[4] = {0xAF, 0x00, 0x00, 0x51}; // Used to store CMD to be sent
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  return readBytes(recBuf, 8);
}

/**********************************************************
Description: Restore module parameters to factory default values
Parameters: None
Return: 0: check ok
        1: check error
        2: timeout error
Others: 1.After the command is executed, the module needs to be preheated again.
        2.Factory set data description:
          (1)Alarm threshold setting: 180ppm
          (2)Exit alarm threshold setting: 55ppm
          (3)Calibration zero point
          (4)Data of the calibration with CO
          (5)Restore factory setting
**********************************************************/
uint8_t BM22S3221_1::restoreDefault()
{
  uint8_t sendBuf[4] = {0xA0, 0x00, 0x00, 0x60};
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(50); // Waiting for module to receive data and reply
  return readBytes(recBuf, 8);
}

/**********************************************************
Description: Get firmware version
Parameters: None
Return: FW version, for example 0x0106: V1.6
Others: None
**********************************************************/
uint16_t BM22S3221_1::getFWVer()
{
  uint16_t ver = 0;
  uint8_t sendBuf[4] = {0xAD, 0x00, 0x00, 0x53};
  uint8_t recBuf[12];
  writeBytes(sendBuf, 4);
  delay(30); // Wait for the module to reply data
  if (readBytes(recBuf, 12) == 0)
  {
    ver = (recBuf[6] << 8) + recBuf[7];
  }
  return ver;
}

/**********************************************************
Description: Get production date
Parameters: array[0]: year
            array[1]: month
            array[2]: day
            (for example 0x22, 0x05, 0x06: 2022/5/6)
Return: 0: check ok
        1: check error
        2: timeout error
Others: None
**********************************************************/
uint8_t BM22S3221_1::getProDate(uint8_t array[])
{
  uint8_t errFlag;
  uint8_t sendBuf[4] = {0xAD, 0x00, 0x00, 0x53};
  uint8_t recBuf[12];
  writeBytes(sendBuf, 4);
  delay(30); // Wait for the module to reply data
  errFlag = readBytes(recBuf, 12);
  if (errFlag == 0)
  {
    for (uint8_t i = 8; i < 11; i++)
    {
      array[i - 8] = recBuf[i];
    }
  }
  return errFlag;
}

/**********************************************************
Description: Query whether the automatic serial output of the module is enabled
Parameters: None
Return: true(1): auto-Tx ENABLED
        false(0): auto-Tx DISENABLED
Others: None
**********************************************************/
bool BM22S3221_1::isAutoTx()
{
  uint8_t tmp = 0;
  uint8_t sendBuf[4] = {0xD0, 0x1B, 0x00, 0x15};
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
  }
  if (tmp == 0x08)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**********************************************************
Description: Get the alarm output level of the STATUS pin
Parameters: None
Return: 08 : STATUS pin is high when alarming, low when not alarming
        00 : STATUS pin is low when alarming, high when not alarming
Others: None
**********************************************************/
uint8_t BM22S3221_1::getStatusPinActiveMode()
{
  uint8_t tmp = 0;
  uint8_t sendBuf[4] = {0xD0, 0x1C, 0x00, 0x14};
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
  }
  return tmp;
}

/**********************************************************
Description: Get the alarm threshold
Parameters: None
Return: the threshold value for generating an alarm, in ppm. Default 180 ppm
Others: None
**********************************************************/
uint16_t BM22S3221_1::getAlarmThreshold()
{
  uint16_t tmp = 0;
  uint8_t sendBuf1[4] = {0xD0, 0x0C, 0x00, 0x24};
  uint8_t sendBuf0[4] = {0xD0, 0x0D, 0x00, 0x23};
  uint8_t recBuf[8];
  writeBytes(sendBuf1, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
    tmp = tmp << 8;
    writeBytes(sendBuf0, 4);
    delay(20); // Wait for the module to reply data
    if (readBytes(recBuf, 8) == 0)
    {
      tmp += recBuf[6];
    }
    else
    {
      tmp = 0;
    }
  }
  return tmp;
}

/**********************************************************
Description: Get the exit alarm threshold
Parameters: None
Return: the value to stop alarming, in ppm.
        When CO concentration is lower than this value, the alarm stops. Default 55 ppm
Others: None
**********************************************************/
uint8_t BM22S3221_1::getExitAlarmThreshold()
{
  uint8_t tmp = 0;
  uint8_t sendBuf[4] = {0xD0, 0x0E, 0x00, 0x22};
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  if (readBytes(recBuf, 8) == 0)
  {
    tmp = recBuf[6];
  }
  return tmp;
}

/**********************************************************
Description: Set whether Tx pin output data automatically
Parameters: autoTx = AUTO(08): enable to transmit the 34-byte module information on Tx pin automatically per second
            autoTx = PASSIVE(00): disable to transmit automatically
Return: 0: check ok
        1: check error
        2: timeout error
Others:   None
**********************************************************/
uint8_t BM22S3221_1::setAutoTx(uint8_t autoTx)
{
  uint8_t sendBuf[4] = {0xE0, 0x1B, 0, 0};
  uint8_t recBuf[8];
  sendBuf[2] = autoTx;
  sendBuf[3] = ~(sendBuf[0] + sendBuf[1] + sendBuf[2]) + 1;
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  return readBytes(recBuf, 8);
}

/**********************************************************
Description: Set the alarm output level of the STATUS pin
Parameters: statusMode = HIGH_LEVEL(08): Status pin is high when alarming, low when not alarming
            statusMode = LOW_LEVEL(00): Status pin is low when alarming, high when not alarming
Return: 0: check ok
        1: check error
        2: timeout error
Others:   None
**********************************************************/
uint8_t BM22S3221_1::setStatusPinActiveMode(uint8_t statusMode)
{
  uint8_t sendBuf[4] = {0xE0, 0x1C, 0, 0};
  uint8_t recBuf[8];
  sendBuf[2] = statusMode;
  sendBuf[3] = ~(sendBuf[0] + sendBuf[1] + sendBuf[2]) + 1;
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  return readBytes(recBuf, 8);
}

/**********************************************************
Description: Set the threshold for module alarms
Parameters: alarmThreshold, in ppm. Default 180 ppm
            When CO concentration is higher than this value, alarm is set.
Return: 0: check ok
        1: check error
        2: timeout error
Others:   None
**********************************************************/
uint8_t BM22S3221_1::setAlarmThreshold(uint16_t alarmThreshold)
{
  uint8_t sendBuf1[4] = {0xE0, 0x0C, 0, 0};
  uint8_t sendBuf0[4] = {0xE0, 0x0D, 0, 0};
  uint8_t recBuf[8], errFlag;
  sendBuf1[2] = alarmThreshold >> 8;
  sendBuf0[2] = alarmThreshold & 0xFF;
  sendBuf1[3] = ~(sendBuf1[0] + sendBuf1[1] + sendBuf1[2]) + 1;
  sendBuf0[3] = ~(sendBuf0[0] + sendBuf0[1] + sendBuf0[2]) + 1;
  writeBytes(sendBuf1, 4);
  delay(20); // Wait for the module to reply data
  errFlag = readBytes(recBuf, 8);
  if (errFlag == 0)
  {
    writeBytes(sendBuf0, 4);
    delay(20); // Wait for the module to reply data
    return readBytes(recBuf, 8);
  }
  else
  {
    return errFlag;
  }
}

/**********************************************************
Description: Set the threshold for module stop alarm
Parameters: exitAlarmThreshold, in ppm. Default 55 ppm
            When CO concentration is lower than this value, stop alarming.
Return: 0: check ok
        1: check error
        2: timeout error
Others:   None
**********************************************************/
uint8_t BM22S3221_1::setExitAlarmThreshold(uint8_t exitAlarmThreshold)
{
  uint8_t sendBuf[4] = {0xE0, 0x0E, 0, 0};
  uint8_t recBuf[8];
  sendBuf[2] = exitAlarmThreshold;
  sendBuf[3] = ~(sendBuf[0] + sendBuf[1] + sendBuf[2]) + 1;
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  return readBytes(recBuf, 8);
}
/**********************************************************
Description: Calibrate module
Parameters: None
Return: 0: check ok
        1: check error
        2: timeout error
Others: 1. Trigger the calibration with gas,
           which needs to be carried out in the standard concentration gas environment,
           (For example, in the gas concentration environment of 10% LEL / 5000ppm)
        2. The module enters the calibration state after receiving this command.
           Do not send other commands to the module during this period
**********************************************************/
uint8_t BM22S3221_1::calibrateModule()
{
  uint8_t sendBuf[4] = {0xAB, 0xB1, 0x00, 0xA4};
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(20); // Wait for the module to reply data
  return readBytes(recBuf, 8);
}

/**********************************************************
Description: Wait for the module calibration to complete(about 120 seconds)
Parameters: None
Return: None
Others: None
**********************************************************/
void BM22S3221_1::calibrateCountdown()
{
  countdown(_calibration);
}

/**********************************************************
Description: Clear UART Receive FIFO
Parameters: None
Return: None
Others: None
**********************************************************/
void BM22S3221_1::clear_UART_FIFO()
{
  if (_softSerial != NULL)
  {
    while (_softSerial->available() > 0)
    {
      _softSerial->read();
    }
  }
  if (_hardSerial != NULL)
  {
    while (_hardSerial->available() > 0)
    {
      _hardSerial->read();
    }
  }
}

/**********************************************************
Description: Write data through UART
Parameters: wbuf:The array for storing Data to be sent
            wlen:Length of data sent
Return: None
Others: None
**********************************************************/
void BM22S3221_1::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
  clear_UART_FIFO();

  /*Select hardSerial or softSerial according to the setting*/
  if (_softSerial != NULL)
  {
    _softSerial->write(wbuf, wlen);
  }
  if (_hardSerial != NULL)
  {
    _hardSerial->write(wbuf, wlen);
  }
}
/**********************************************************
Description: Read data through UART
Parameters: rbuf: Used to store received data
            rlen: Length of data to be read
Return: 0: check ok
        1: check error
        2: timeout error
Others: None
**********************************************************/
uint8_t BM22S3221_1::readBytes(uint8_t rbuf[], uint8_t rlen, uint16_t timeout)
{
  uint8_t i, delayCnt, checkCode;

  /* Select hardSerial or softSerial according to the setting,
     check if there is data in the UART Receive FIFO */
  if (_softSerial != NULL)
  {
    for (i = 0; i < rlen; i++)
    {
      delayCnt = 0;
      while (_softSerial->available() == 0)
      {
        if (delayCnt > timeout)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _softSerial->read(); // Receive data
    }
  }
  else if (_hardSerial != NULL)
  {
    for (i = 0; i < rlen; i++)
    {
      delayCnt = 0;
      while (_hardSerial->available() == 0)
      {
        if (delayCnt > timeout)
        {
          return TIMEOUT_ERROR; // Timeout error
        }
        delay(1);
        delayCnt++;
      }
      rbuf[i] = _hardSerial->read(); // Receive data
    }
  }

  /* Calculate check code*/
  for (i = 0, checkCode = 0; i < (rlen - 1); i++)
  {
    checkCode += rbuf[i];
  }
  checkCode = ~checkCode + 1;

  /* Proofread check code*/
  if (checkCode == rbuf[rlen - 1])
  {
    return CHECK_OK; // Check correct
  }
  else
  {
    return CHECK_ERROR; // Check error
  }
}

/**********************************************************
Description: Countdown
Parameters: 1: Perform preheat countdown
            0: Perform calibration countdown
Return: None
Others: None
**********************************************************/
void BM22S3221_1::countdown(uint8_t type)
{
  uint8_t countdown, tmp = 0, i;
  uint8_t delayTime = 120, num = 14, txMode = AUTO;
  uint8_t recBuf[32] = {0}, recLen = 32;
  uint16_t delayCnt = 0;

  if (type == _preheat)
  {
    delayTime = _preheatTime;
    num = 14;
  }
  else if (type == _calibration)
  {
    delayTime = _calTime;
    num = 15;
  }
  do
  {
    delayCnt = 0;
    tmp = 0;
    clear_UART_FIFO();
    while (tmp < recLen)
    {
      if (_softSerial != NULL) // Select hardSerial or softSerial according to the setting
      {
        tmp = _softSerial->available();
      }
      else if (_hardSerial != NULL)
      {
        tmp = _hardSerial->available();
      }
      delay(1);
      delayCnt++;
      if (delayCnt > 3000)
      {
        txMode = PASSIVE; // 32-byte of data are not received within 3 seconds
        break;
      }
    }
    if (txMode == PASSIVE)
    {
      break; // Exit the do {...} While loop
    }
    if (readBytes(recBuf, recLen) == 0)
    {
      countdown = recBuf[num];
    }
  } while (countdown != 0);

  /* auto-Tx DISENABLED, delay "delayTime" sec*/
  if (txMode == PASSIVE)
  {
    for (i = delayTime; i > 0; i--)
    {
      delay(1030);
    }
  }
}