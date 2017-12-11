/*
 *  responceParser.h
 *
 *  Created on: 18 oct 2017
 *      Author: PZ
 */

#ifndef RESPONSE_PARSER_H
#define RESPONSE_PARSER_H
#include <string>
#include <iostream>
#include <vector>
#include "types.h"

#define POSITIVE_RES       0x00
#define SUCCESS            0x00
#define NEGATIVE_RES       0x01
#define CANNOT_FIND_DLL    0x02
#define CANNOT_OPEN_FILE   0x03

class responseParser {
private:
  std::vector <u64> positiveResults;
  std::vector <int> negativeResults;
  char responseStatus;
public:
  char parseOutput(char* response);
  u64 getPosResValue(unsigned char index);
  char getPosResSize(void);
  char getStatus(void);
  int getNegResValue(unsigned char index);
  char getNegResSize(void);
  ~responseParser();
};

#endif
