#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <conio.h>
#include "peltierUser.h"

extern bool TECcfg;
bool TECon = false;
int mtemp = 50;
char retStat = NEGATIVE_RES;

using namespace std;

void SetPID(cmdHandler*xtplPrinter, int Ppar, int Ipar, int Dpar)
{
	char Pch[10];
	char Ich[10];
	char Dch[10];
	itoa(Ppar, Pch, 10);
	itoa(Ipar, Ich, 10);
	itoa(Dpar, Dch, 10);
	string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 2";
	cmd.append(" ");
	cmd.append(Pch);
	cmd.append(" ");
	cmd.append(Ich);
	cmd.append(" ");
	cmd.append(Dch);
	xtplPrinter->sendCmd((char*)cmd.c_str());
}

void SetTemp(cmdHandler*xtplPrinter, int temp)
{
	char Tempch[10];
	itoa(temp, Tempch, 10);
	string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 1";
	cmd.append(" ");
	cmd.append(Tempch);
	xtplPrinter->sendCmd((char*)cmd.c_str());
}

void ReadTemp(cmdHandler*xtplPrinter)
{
	retStat = xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 0");
	if (POSITIVE_RES == retStat)
		cout << "Actual T: " << xtplPrinter->getPosResValue(0) << " *C" << endl;
		//string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 0";;
		//xtplPrinter->sendCmd((char*)cmd.c_str());
}

void peltierUser(cmdHandler*xtplPrinter) {
	SetPID(xtplPrinter, 15000, 50, 3500);
	ifstream menuTEC;
	menuTEC.open("menuTec.txt");
	cout << menuTEC.rdbuf() << endl;
	menuTEC.close();
	while (TECcfg == true)
		switch (getch()) {
		case 'a':
			TECon = true;
			cout << "Temperature set to: " << mtemp << " *C ";
			ReadTemp(xtplPrinter);
			SetTemp(xtplPrinter, mtemp);
			break;
		case 'k':
			mtemp--;
			if (mtemp < 1) 
			{
				mtemp = 1;
			}
			cout << "Target T: " << mtemp << " ";
			ReadTemp(xtplPrinter);
			if (TECon == true)
			{
				SetTemp(xtplPrinter, mtemp);
			}
			break;
		case 'l':
			mtemp++;
			if (mtemp > 90)
			{
				mtemp = 90;
			}
			cout << "Target T: " << mtemp << " ";
			ReadTemp(xtplPrinter);
			if (TECon == true)
			{
				SetTemp(xtplPrinter, mtemp);
			}
			break;
		case 's':
			cout << "TEC turned off\n";
			SetTemp(xtplPrinter, 0);
			TECon = false;
			break;
		case 'd':
			cout << "Target T: " << mtemp << " ";
			ReadTemp(xtplPrinter);
			break;
		case 27: // ESC
			cout << "Leaving TEC control menu...\n";
			TECcfg = false;
			Sleep(500);
			break;

		default:
			break;
		}
}