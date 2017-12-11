/*
 *  cmdHandler.h
 *
 *  Created on: 18 oct 2017
 *      Author: PZ
 */
#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H
#include <ws2spi.h>
#include <stdio.h>
#include "responseParser.h"
#include "types.h"

typedef void (*IcsClientExecA_)(char* command, char *response, unsigned long responeSize, unsigned char socket_mode);

#define COMMAND_BUFFER_SIZE  10000
#define RESPONSE_BUFFER_SIZE 10000
#define SOCKET_MODE_DO_NOT_CLOSE_CONNECTION 0
#define SOCKET_MODE_CLOSE_CONNECTION        1

#define BOOTLOADER_MODE 0x1000

class cmdHandler {
private:
  std::string releasePath;
  std::string deviceName;
  HINSTANCE hLib;
  IcsClientExecA_ IcsClientExecA;
  unsigned long CommandCurrentPosition;
  responseParser cmdRes;
  PROCESS_INFORMATION processInfo;
  void flashDevice(void);
  void serverHandle(void);
  void StdCout(const char *msg);
  unsigned int getFileSize(const char*);
public:
  cmdHandler(char* path, char* name);
  ~cmdHandler();
  char sendCmd(const char* cmd, char printCmd=FALSE);
  u64 getPosResValue(unsigned char index);
  char getPosResSize(void);
  int  getNegResValue(unsigned char index);
  char getNegResSize(void);
  char getStatus(void);
};

#endif