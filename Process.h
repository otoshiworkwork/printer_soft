#ifndef MOVE_PROCESS_H
#define MOVE_PROECSS_H
#include "cmdHandler.h"

void AdvancedControl(cmdHandler*);
void PrintProcess(cmdHandler*, int printX, int printA, long long int printtime);
bool ZAdjust(cmdHandler*, int safeZ, int relay);
void AutoDispensing(cmdHandler *, int xStartPos, int yStartPos, int zStartPos, int StartingPad, int EndingPad);
void AutoPrinting(cmdHandler *, int xStartPos, int yStartPos, int zStartPos, int StartingPad, int EndingPad, int Vac, int Vdc, int freq);
int SleepOrSkip(cmdHandler* , int ms100);
void Softoff(cmdHandler*, float Vacdecr, int SlopeTime1, int WaitTime, int SlopeTime2);
int RelayMask(cmdHandler *, int relay);
bool ContactCheck(cmdHandler*, bool response, int relay);

#endif
