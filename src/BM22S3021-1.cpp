/*****************************************************************
File:        BM22S3021-1.cpp
Author:      BESTMODULES
Description: None
History:     None
V1.0.1 --Initial version; 2023-3-14; Arduino IDE : v1.8.19
******************************************************************/
#include "BM22S3021-1.h"

/**********************************************************
Description: Constructor
Parameters: statusPin: Status pin connection with Arduino
            *theSerial: Serial object if your board has more than one UART interface
Output: None
Return: None
Others: None
**********************************************************/
BM22S3021_1::BM22S3021_1(uint8_t statusPin, HardwareSerial *theSerial)
{
  _statusPin = statusPin;
  _hardSerial = theSerial;
}

/**********************************************************
Description: Module initialization
Parameters: None
Return: None
Others: None
**********************************************************/
void BM22S3021_1::begin()
{
  _hardSerial->begin(BAUDRATE); // baud rate:9600
  pinMode(_statusPin, INPUT);
}

/**********************************************************
Description: Preheat Module(about 180 seconds)
Parameters: None
Return: None
Others: None
**********************************************************/
void BM22S3021_1::preheatCountdown()
{
  uint8_t recBuf[18], time = 180;
  delay(1500);
  if (isInfoAvailable() == true)
  {
    while (time > 0)
    {
      if (isInfoAvailable() == true)
      {
        readInfoPackage(recBuf);
        time = recBuf[8];
      }
      delay(1030);
    }
  }
  else
  {
    while (time > 0)
    {
      time = readParam(0xD2, 0x51); // Query remaining preheat time
      delay(3000);
    }
  }
}

/**********************************************************
Description: Get STATUS pin level
Parameters: None
Return: None
Others: None
**********************************************************/
uint8_t BM22S3021_1::getSTATUS()
{
  return digitalRead(_statusPin);
}

/**********************************************************
Description: Get the current status of the module
Parameters: None
Return: module status (refer to datasheet for meaning of each bit)
Others: None
**********************************************************/
uint8_t BM22S3021_1::getWorkStatus()
{
  return readParam(0xD2, 50);
}

/**********************************************************
Description: Query gas real-time A/D value
Parameters: None
Return: 8-bit A/D value
Others: None
**********************************************************/
uint8_t BM22S3021_1::readADValue()
{
  return readParam(0xD2, 40);
}

/**********************************************************
Description: Query the calibrated gas alarm point
Parameters: None
Return: 8-bit A/D value
Others: None
**********************************************************/
uint8_t BM22S3021_1::readAlarmPoint()
{
  return readParam(0xD2, 41);
}

/**********************************************************
Description: Get the current status and data of the module
Parameters: array: The array for storing the 18-byte module information
                  (refer to datasheet for meaning of each bit)
Return: 0: Request ok
        1: Check error
        2: Timeout error
Others: None
**********************************************************/
uint8_t BM22S3021_1::requestInfoPackage(uint8_t array[])
{
  uint8_t i, errFlag = 1;
  uint8_t sendBuf[4] = {0xAC, 0x00, 0x00, 0x54};
  uint8_t recBuf[18];
  writeBytes(sendBuf);
  delay(1); // Wait for the module to reply data
  errFlag = readBytes(recBuf, 18);
  if (errFlag == CHECK_OK)
  {
    for (i = 0; i < 18; i++)
    {
      array[i] = recBuf[i];
    }
  }
  else
  {
    for (i = 0; i < 18; i++)
    {
      array[i] = 0;
    }
  }
  return errFlag;
}

/**********************************************************
Description: Query whether the 18-byte data sent by the module is received
Parameters: None
Return: TRUE(1): 18-byte data received
        FALSE(0): 18-byte data not received
Others: Only used in the mode of Tx Auto Output Info
**********************************************************/
bool BM22S3021_1::isInfoAvailable()
{
  uint8_t header[5] = {0xAA, 0x12, 0x02, 0x01, 0xAC}; // Fixed code for first 5 bytes of 18-byte data
  uint8_t recBuf[18] = {0}, recLen = 18;
  uint8_t i, num = 0, readCnt = 0, failCnt = 0, checkSum = 0;
  bool isHeader = false, result = false;

  /* Serial buffer contains at least one 18-byte data */
  num = _hardSerial->available();
  if (num >= recLen)
  {
    while (failCnt < 3) // Didn't read the required data twice, exiting the loop
    {
      /* Find 5-byte data header */
      for (i = 0; i < 5;)
      {
        recBuf[i] = _hardSerial->read();
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
          checkSum += recBuf[i]; // Sum checkSum
        }
        for (i = 5; i < recLen; i++) // Read subsequent 27-byte data
        {
          recBuf[i] = _hardSerial->read();
          checkSum += recBuf[i]; // Sum checkSum
        }
        checkSum = checkSum - recBuf[recLen - 1];
        checkSum = (~checkSum) + 1; // Calculate checkSum

        /* Compare whether the check code is correct */
        if (checkSum == recBuf[recLen - 1])
        {
          for (i = 0; i < recLen; i++)
          {
            _recBuf[i] = recBuf[i]; // True, assign data to _recBuf[]
          }
          result = true;
          break; // Exit "while (failCnt < 3)" loop
        }
        else
        {
          failCnt++; // Error, failCnt plus 1, return "while (failCnt < 3)" loop
          checkSum = 0;
        }
      }
    }
  }
  return result;
}

/**********************************************************
Description: Read the 18-byte data of sent by the module
Parameters: array: The array for storing the 18-byte module information
                  (refer to datasheet for meaning of each bit)
Return: None
Others: Use after isInfoAvailable()
**********************************************************/
void BM22S3021_1::readInfoPackage(uint8_t array[])
{
  for (uint8_t i = 0; i < 18; i++)
  {
    array[i] = _recBuf[i];
  }
}

/**********************************************************
Description: Software reset module
Parameters: None
Return: 0: check ok
        1: check error
        2: timeout error
Others: After the command is executed, the module needs to be preheated again
**********************************************************/
uint8_t BM22S3021_1::resetModule()
{
  uint8_t sendBuf[4] = {0xAF, 0x00, 0x00, 0x51}; // Used to store CMD to be sent
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(1); // Wait for the module to reply data
  return readBytes(recBuf, 8);
}

/**********************************************************
Description: Restore module parameters to factory default values
Parameters: None
Return: 0: Check ok
        1: Check error
        2: Timeout error
Others: 1.After the command is executed, the module needs to be preheated again.
        2.Factory set data description:
          (1)Calibrated alarm value: 9% LEL A/D value
          (2)Power-on warm-up time: 180s
          (3)Calibration time: 150s
          (4)Serial port output: enable
          (5)Alarm output level: STATUS port output high when alarmed, normal is low
          (6)Calibration minimum limit: 77 A/D values
          (7)Calibration maximum limit: 205 A/D values
**********************************************************/
uint8_t BM22S3021_1::restoreDefault()
{
  uint8_t sendBuf[4] = {0xA0, 0x00, 0x00, 0x60};
  uint8_t recBuf[8];
  writeBytes(sendBuf, 4);
  delay(1); // Waiting for module to receive data and reply
  return readBytes(recBuf, 8);
}

/**********************************************************
Description: Read parameter from module
Parameters: cmd: Command code(8-bit)
            addr: Address code(8-bit)
Return: Module parameter
Others: 1. Consult the command table of the BM22S3021-1 datasheet
        2. Not applicable to "Common Command"
**********************************************************/
uint8_t BM22S3021_1::readParam(uint8_t cmd, uint8_t addr)
{
  uint8_t sendBuf[4] = {cmd, addr, 0x00, 0x00};
  uint8_t recBuf[8] = {0};
  sendBuf[3] = ~(sendBuf[0] + sendBuf[1] + sendBuf[2]) + 1;
  writeBytes(sendBuf);
  delay(1); // Wait for the module to reply data
  if (readBytes(recBuf) == CHECK_OK)
  {
    return recBuf[6];
  }
  else
  {
    return 0;
  }
}

/**********************************************************
Description: Get firmware version
Parameters: None
Return: FW version, for example 0x0101: V1.01
Others: None
**********************************************************/
uint16_t BM22S3021_1::getFWVer()
{
  uint16_t ver = 0;
  uint8_t sendBuf[4] = {0xAD, 0x00, 0x00, 0x53};
  uint8_t recBuf[12];
  writeBytes(sendBuf, 4);
  delay(1); // Wait for the module to reply data
  if (readBytes(recBuf, 12) == CHECK_OK)
  {
    ver = ((uint16_t)recBuf[6] << 8) + recBuf[7];
  }
  return ver;
}

/**********************************************************
Description: Get production date
Parameters: array[0]: year
            array[1]: month
            array[2]: day
Return: 0: Check ok
        1: Check error
        2: Timeout error
Others: For example, array[] = {0x19,0x07,0x23},
        production date: July 23, 2019.
**********************************************************/
uint8_t BM22S3021_1::getProDate(uint8_t array[])
{
  uint8_t errFlag = 1;
  uint8_t sendBuf[4] = {0xAD, 0x00, 0x00, 0x53};
  uint8_t recBuf[12];
  writeBytes(sendBuf, 4);
  delay(1); // Wait for the module to reply data
  errFlag = readBytes(recBuf, 12);
  for (uint8_t i = 0; i < 3; i++)
  {
    if (errFlag == CHECK_OK)
    {
      array[i] = recBuf[i + 8];
    }
    else
    {
      array[i] = 0;
    }
  }
  return errFlag;
}

/**********************************************************
Description: Query whether the automatic serial output of the module is enabled
Parameters: None
Return: TRUE(1): auto-Tx ENABLED
        FALSE(0): auto-Tx DISENABLED
Others: None
**********************************************************/
bool BM22S3021_1::isAutoTx()
{
  if (readParam(0xD0, 0x1B) == 0x08)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**********************************************************
Description: Write command to module
Parameters: cmd: Command code(8-bit)
            addr: Address code(8-bit)
            param: Parameters to be written(8-bit)
Return: 0: Write Successful
        1: Check error
        2: Timeout error
Others: 1. Consult the command table of the BM22S3021-1 datasheet
        2. Not applicable to "Common Command"
**********************************************************/
uint8_t BM22S3021_1::writeCommand(uint8_t cmd, uint8_t addr, uint8_t param)
{
  uint8_t sendBuf[4] = {cmd, addr, param, 0x00};
  uint8_t recBuf[8] = {0};
  sendBuf[3] = ~(sendBuf[0] + sendBuf[1] + sendBuf[2]) + 1;
  writeBytes(sendBuf);
  delay(1); // Wait for the module to reply data
  return readBytes(recBuf);
}

/**********************************************************
Description: Set whether Tx pin output data automatically
Parameters: autoTx = AUTO(08): enable to transmit the 18-byte module information
                               on Tx pin automatically per second
            autoTx = PASSIVE(00): disable to transmit automatically
Return: 0: check ok
        1: check error
        2: timeout error
Others: None
**********************************************************/
uint8_t BM22S3021_1::setAutoTx(uint8_t autoTx)
{
  return writeCommand(0xE0, 0x1B, autoTx);
}

/**********************************************************
Description: Calibrate module
Parameters: calibrateMode = 0xB0: Trigger air calibration
            calibrateMode = 0xB1: Trigger calibration with gas
Return: 0: Write Successful
        1: Check error
        2: Timeout error
Others: 1. When the air calibration is triggered,
           the equipment will set a default alarm value,
           which does not need to be carried out in the standard concentration gas environment.
        2. When the calibration with gas is triggered,
           which needs to be carried out in the standard concentration gas environment.
        3. The module enters the calibration state after receiving this command.
           Do not send other commands to the module during this period
**********************************************************/
uint8_t BM22S3021_1::calibrateModule(uint8_t calibrateMode)
{
  return writeCommand(0xAB, calibrateMode, 0x00);
}

/**********************************************************
Description: Clear UART Receive FIFO
Parameters: None
Return: None
Others: None
**********************************************************/
void BM22S3021_1::clear_UART_FIFO()
{
  while (_hardSerial->available() > 0)
  {
    _hardSerial->read();
  }
}

/**********************************************************
Description: Write data through UART
Parameters: wbuf:The array for storing Data to be sent
            wlen:Length of data sent
Return: None
Others: None
**********************************************************/
void BM22S3021_1::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
  clear_UART_FIFO();
  _hardSerial->write(wbuf, wlen);
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
uint8_t BM22S3021_1::readBytes(uint8_t rbuf[], uint8_t rlen, uint16_t timeout)
{
  uint8_t i = 0, delayCnt = 0, checkSum = 0;

  /* HardwareSerial Interface */

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
    rbuf[i] = _hardSerial->read();
  }

  /* check Sum */
  for (i = 0; i < (rlen - 1); i++)
  {
    checkSum += rbuf[i];
  }
  checkSum = ~checkSum + 1;
  if (checkSum == rbuf[rlen - 1])
  {
    return CHECK_OK; // Check correct
  }
  else
  {
    return CHECK_ERROR; // Check error
  }
}
