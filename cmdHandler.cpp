/*
 *  cmdHandler.cpp
 *
 *  Created on: 18 oct 2017
 *      Author: PZ
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include "cmdHandler.h"


/* Constructor */
cmdHandler::cmdHandler(char* path, char* name) {
  /* Init private viariables*/
  releasePath = path;
  deviceName  = name;
  
  /* Create path to *.dll file */
  std::string dllpath = path;
  dllpath.append("\\ics_client\\lib\\ics_client.dll");
  hLib=LoadLibrary(dllpath.c_str());
  
  if(hLib != NULL) {
    IcsClientExecA = (IcsClientExecA_)GetProcAddress((HMODULE)hLib, "IcsClientExecA");
    if(IcsClientExecA != NULL) {
      serverHandle();
      flashDevice();
    }
  }
  else {
    StdCout("E.Error: Cannot load library: ics_client.dll ");
  }
}
/* Destructor */
cmdHandler::~cmdHandler() {
  std::string cmd;
  cmd.append("ics_client.exe --dev_close_serial 127.0.0.1 42100 ");
  cmd.append(deviceName);
  sendCmd((char*)cmd.c_str(), TRUE);
  FreeLibrary((HMODULE)hLib);
  //Terminate Server API
  TerminateProcess(processInfo.hProcess, 0);
  TerminateProcess(processInfo.hThread, 0);
}

void cmdHandler::flashDevice(void) {
  /* Check if device was already programmed*/
  std::stringstream cmd;
  std::string filePath;
  cmd << "ics_client.exe --dev_open_serial 127.0.0.1 42100 " << deviceName;
  sendCmd(cmd.str().c_str());
  cmd.str("");
  /* Check firmware version*/
  if(POSITIVE_RES == sendCmd("ics_client.exe --dev_software_version 127.0.0.1 42100")){
    if(BOOTLOADER_MODE == getPosResValue(0)) {
      std::cout << "V#: Programming XTPL Printer..." << std::endl;
      sendCmd("ics_client.exe --sys_config_buffer   127.0.0.1 42100 --config 2 2000000");
      sendCmd("ics_client.exe --dev_jump_to_address 127.0.0.1 42100 0x0");
      filePath = releasePath;
      filePath.append("\\xtpl_app\\xtpl_printer\\base\\001\\firmware\\prt_core.ifh");
      cmd << "ics_client.exe --sys_client_wr_rd_buffer 127.0.0.1 42100 --wr_s 0 0 \""
          << filePath;
      sendCmd(cmd.str().c_str());
      cmd.str("");
      cmd << "ics_client.exe --dev_config_hardware   127.0.0.1 42100 --pd 0 0 "
          << getFileSize(filePath.c_str());
      sendCmd(cmd.str().c_str());
      cmd.str("");
      filePath = releasePath;
      filePath.append("\\xtpl_app\\xtpl_printer\\base\\001\\firmware\\prt_soft.ifs");
      cmd << "ics_client.exe --sys_client_wr_rd_buffer 127.0.0.1 42100 --wr_s 0 0 \""
          << filePath;
      sendCmd(cmd.str().c_str());
      cmd.str("");
      cmd << "ics_client.exe --dev_wr_rd_buffer    127.0.0.1 42100 --wr_s 0 0 0x40000000 "
          << getFileSize(filePath.c_str());
      sendCmd(cmd.str().c_str());
      sendCmd("ics_client.exe --dev_jump_to_address 127.0.0.1 42100 0x40000000");
    }
  }
}

char cmdHandler::sendCmd(const char* cmd, char printCmd) {
  char response [RESPONSE_BUFFER_SIZE];
  char retVal = NEGATIVE_RES;
   
  if((IcsClientExecA != NULL) && (hLib != NULL)){
    IcsClientExecA((char*)cmd, response, RESPONSE_BUFFER_SIZE, SOCKET_MODE_DO_NOT_CLOSE_CONNECTION);
    retVal = cmdRes.parseOutput(response);
    if(retVal == NEGATIVE_RES){
      /* Print cmd and negative response on stdout */
      StdCout(cmd);
      StdCout("\r\n");
      StdCout(response);
    }
    else {
      if(TRUE == printCmd) {
        StdCout(cmd);
        StdCout("\r\n");
        StdCout(response);
      }
    }
  }
  else {
    retVal = CANNOT_FIND_DLL;
  }
  return retVal;
}

void cmdHandler::StdCout(const char *msg){
  WORD color = FOREGROUND_BLUE|FOREGROUND_GREEN;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
  GetConsoleScreenBufferInfo(hConsole, &csbiInfo);
  if (strlen(msg) >= 2){
    if ((msg[0] == 'E') && (msg[1] == '.')){
      color = FOREGROUND_RED | FOREGROUND_INTENSITY;
    }
  }
  SetConsoleTextAttribute(hConsole, color);
  printf("%s",msg);
  SetConsoleTextAttribute(hConsole, csbiInfo.wAttributes);
}

u64 cmdHandler::getPosResValue(unsigned char index) {
  return cmdRes.getPosResValue(index);
}

char cmdHandler::getPosResSize(void){
  return cmdRes.getPosResSize();
}

int cmdHandler::getNegResValue(unsigned char index) {
  return cmdRes.getNegResValue(index);
}

char cmdHandler::getNegResSize(void){
  return cmdRes.getNegResSize();
}

char cmdHandler::getStatus(void) {
  return cmdRes.getStatus();
}

unsigned int cmdHandler::getFileSize(const char* fileName) {
  int fileSize;
  std::ofstream file;
  file.open(fileName, std::fstream::in);
  if(file.good() == TRUE) {
    file.seekp(0, std::ios::end);
    fileSize = file.tellp();
  }else {
    std::cout << "V#: Error: Can't find release files!"<<std::endl; 
  }
  file.close();
  return fileSize;
}

void cmdHandler::serverHandle(void) {

  std::string filePath = releasePath;
  STARTUPINFO info={sizeof(info)};

  filePath.append("\\ics_server\\ics_server.exe --ics32sx --connect 42100 --privileged");
  info.hStdOutput = NULL;
  info.dwFlags    = STARTF_USESTDHANDLES; 
  int retVal  =  CreateProcess(NULL, (LPSTR)filePath.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
  
  if (retVal == 0) {
    std::cout << "V#: Server Process Error: Press \"Enter\" to exit.." <<std::endl;
  }

}