#ifndef DAC_TEST_H
#define DAC_TEST_H
#include "cmdHandler.h"

void SetRelayManual(cmdHandler*, int ResistorValue);
void SetRelayAuto(cmdHandler*, int RelayNumber, int ResistorValue);

#endif