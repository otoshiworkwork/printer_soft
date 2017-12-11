#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <conio.h>
#include "moveGeneral.h"
#include "moveHoming.h"
#include "movePrint.h"

extern int axispos;
extern int xpos;
extern int ypos;
extern int zpos;
extern int apos;
extern char axis;

using namespace std;

void moveHoming(cmdHandler*xtplPrinter, char axis)
{
	
	cout << "\nHoming axis: " << axis << endl;
	Sleep(1000);
	
	cout << "Quick move\n";
	Move(xtplPrinter, axis, 1, 3000);
	cout << "[press any button after axis stops]\n";
	getch();

	SetCurrPosition(xtplPrinter, axis, 10000);
	Sleep(200);

	Move(xtplPrinter, axis, 15000, 10000);
	Sleep(2000);

	cout << "Slow move\n";
	Move(xtplPrinter, axis, 1, 30000);
	Sleep(2500);

	SetCurrPosition(xtplPrinter, axis, 10000);
	Sleep(200);

	Move(xtplPrinter, axis, 20000, 30000);
	Sleep(2300);
	cout << "Axis " << axis << " homed." << endl;
}