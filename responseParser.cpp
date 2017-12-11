/*
 *  responceParser.cpp
 *
 *  Created on: 18 oct 2017
 *      Author: PZ
 */
#include "responseParser.h"

char responseParser::parseOutput(char* response) {

  char delimiterPos = 0;
  std::string buff = response;
  std::string variableBuff;
  /* Clear previous converion  of positive/negative Results */
  positiveResults.clear();
  negativeResults.clear();
  /* Take care of only first line of response */
  delimiterPos = buff.find('\n');
  buff = buff.substr(0, delimiterPos);

  /* Check response status*/
  if(buff[0] == 'E') {
    responseStatus = NEGATIVE_RES;
    /* Rewind buff to skip status chars*/
    buff = buff.substr(buff.find(' ') + 1, delimiterPos);
    /* Collect Error response numbers*/
    while ((delimiterPos = buff.find(' ')) != std::string::npos) {
      /* Catch next ASCI variable*/
      variableBuff = buff.substr(0, delimiterPos);
      negativeResults.push_back(atoi(variableBuff.c_str()));
      buff.erase(0, delimiterPos + 1);
    }
    negativeResults.push_back(atoi(buff.c_str()));
  }
  else if(buff[0] == 'O') {
    responseStatus = POSITIVE_RES;
    /* Rewind buff to skip status chars*/
    buff = buff.substr(buff.find(' ') + 1, delimiterPos);
    /* Start getting response parameters*/
    while ((delimiterPos = buff.find(' ')) != std::string::npos) {
      /* Catch next ASCI variable*/
      variableBuff = buff.substr(0, delimiterPos);
      if(std::string::npos == variableBuff.find("0x"))
        positiveResults.push_back(_strtoui64(variableBuff.c_str(), NULL, 0));
      else
        positiveResults.push_back(strtol(variableBuff.c_str(), NULL, 0));
      buff.erase(0, delimiterPos + 1);
    }
    if(std::string::npos == variableBuff.find("0x"))
      positiveResults.push_back(_strtoui64(buff.c_str(), NULL, 0));
    else
      positiveResults.push_back(strtol(buff.c_str(), NULL, 0));
  }
  return responseStatus;
}

long long  unsigned int responseParser::getPosResValue(unsigned char index){
  if(index < getPosResSize())
    return positiveResults[index];
  else
    return 0;
}

char responseParser::getPosResSize(void){
  return positiveResults.size();
}

char responseParser::getStatus(void) {
  return responseStatus;
}

int responseParser::getNegResValue(unsigned char index){
  if(index < getNegResSize())
    return negativeResults[index];
  else
    return 0;
}

char responseParser::getNegResSize(void){
  return negativeResults.size();
}

responseParser::~responseParser() {
  positiveResults.resize(0);
  negativeResults.resize(0);
}