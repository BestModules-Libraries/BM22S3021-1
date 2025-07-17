/*****************************************************************
File:        BM22S3021-1.h
Author:      BEST MODULES CORP.
Description: Define classes and required variables
History:     None
Version:     V1.0.2   -- 2025-07-16
******************************************************************/
#ifndef _BM22S3021_1_H_
#define _BM22S3021_1_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

#define BAUDRATE 9600
#define AUTO 0x08
#define PASSIVE 0x00
#define CHECK_OK 0
#define CHECK_ERROR 1
#define TIMEOUT_ERROR 2

class BM22S3021_1
{
public:
  BM22S3021_1(uint8_t statusPin, HardwareSerial *theSerial = &Serial);
  void begin();
  void preheatCountdown();
  uint8_t getSTATUS();
  uint8_t getWorkStatus();
  uint8_t readADValue();
  uint8_t readAlarmPoint();
  uint8_t requestInfoPackage(uint8_t array[]);
  bool isInfoAvailable();
  void readInfoPackage(uint8_t array[]);
  uint16_t getFWVer();
  uint8_t getProDate(uint8_t array[]);
  uint8_t resetModule();
  uint8_t restoreDefault();
  uint8_t calibrateModule(uint8_t calibrateMode);

  uint8_t readParam(uint8_t cmd, uint8_t addr);
  bool isAutoTx();

  uint8_t writeCommand(uint8_t cmd, uint8_t addr, uint8_t param);
  uint8_t setAutoTx(uint8_t autoTx = AUTO);

private:
  uint8_t _statusPin, _rxPin, _txPin;
  uint8_t _recBuf[18] = {0}; // Array for storing received data
  void clear_UART_FIFO();
  void writeBytes(uint8_t wbuf[], uint8_t wlen = 4);
  uint8_t readBytes(uint8_t rbuf[], uint8_t rlen = 8, uint16_t timeout = 5);
  HardwareSerial *_hardSerial = NULL;
  SoftwareSerial *_softSerial = NULL;
};

#endif
