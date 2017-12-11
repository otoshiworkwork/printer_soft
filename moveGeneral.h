#ifndef MOVE_GENERAL_H
#define MOVE_GENERAL_H
#include "cmdHandler.h"

void moveGeneral(cmdHandler*);
void Move(cmdHandler*, char axis, int dest, int speed);
char* AxisToMask(cmdHandler*, char axis);
void MoveInit(cmdHandler*, int axis, int mode);
void SetCurrPosition(cmdHandler*, char axis, int currposition);
void SetLimiters(cmdHandler*, char axis, int nSTP, int nDIR, int nLim0, int nLim1, int setLim0, int setLim1);
void SetMaxPosition(cmdHandler*, char axis, int maxposition);
void SetTiming(cmdHandler*, char axis, int speed);

void SetMove(cmdHandler*, char axis, int dest, int speed);
void smb_start_move(cmdHandler*xtplPrinter);
void smb_stop_move(cmdHandler*xtplPrinter);

#endif