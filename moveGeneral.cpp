#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <conio.h>
#include "moveGeneral.h"
#include "moveHoming.h"
#include "movePrint.h"
#include "variables.h"
#include "posToFile.h"

extern int dest;
extern int xpos;
extern int ypos;
extern int zpos;
extern int apos;
extern char axis;

char* AxisToMask(cmdHandler*xtplPrinter, char axis) {
	char axisch[10];
	char* mask = axisch;
	switch (axis)
	{
	case 'y':
		mask = "0x01";
		break;
	case 'x':
		mask = "0x02";
		break;
	case 'z':
		mask = "0x04";
		break;
	case 'a':
		mask = "0x08";
		break;
	}
	return mask;
}

void SetCurrPosition(cmdHandler*xtplPrinter, char axis, int currposition) {
	char* mask = AxisToMask(xtplPrinter, axis);
	char currpositionch[10];
	itoa(currposition, currpositionch, 10);
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_set_cur_position ";
	cmd.append(mask);
	cmd.append(" ");
	cmd.append(currpositionch);
	//std::cout << cmd << std::endl;
	xtplPrinter->sendCmd((char*)cmd.c_str());
}

void SetMaxPosition(cmdHandler*xtplPrinter, char axis, int maxposition) {
	char* mask = AxisToMask(xtplPrinter, axis);
	char maxpositionch[10];
	itoa(maxposition, maxpositionch, 10);
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_set_max_position ";
	cmd.append(mask);
	cmd.append(" ");
	cmd.append(maxpositionch);
	xtplPrinter->sendCmd((char*)cmd.c_str());
}

void SetLimiters(cmdHandler*xtplPrinter, char axis, int nSTP, int nDIR, int nLim0, int nLim1, int setLim0, int setLim1) {
	char* mask = AxisToMask(xtplPrinter, axis);
	int value = nSTP + 2 * nDIR + 4 * nLim0 + 8 * nLim1 + 16 * setLim0 + 32 * setLim1;
	char limitersmask[10];
	itoa(value, limitersmask, 16);
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_set_limiters ";
	cmd.append(mask);
	cmd.append(" ");
	cmd.append("0x");
	cmd.append(limitersmask);
	xtplPrinter->sendCmd((char*)cmd.c_str());
}

void SetTiming(cmdHandler*xtplPrinter, char axis, int speed) {
	char* mask = AxisToMask(xtplPrinter, axis);
	char speedch[10];
	itoa(speed, speedch, 10);
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_set_timing ";
	cmd.append(mask);
	cmd.append(" 300 ");
	cmd.append(speedch);
	cmd.append(" 100 100");
	xtplPrinter->sendCmd((char*)cmd.c_str());
}

void MoveInit(cmdHandler*xtplPrinter, int axis, int mode)
{
	switch (mode)
	{
	case 1: //MODE FOR TESTING
		switch (axis)
		{
		case 'y':
			// Y
			SetTiming(xtplPrinter,'y', 20000);
			SetMaxPosition(xtplPrinter, 'y', 10000000);
			SetCurrPosition(xtplPrinter, 'y', 5000000);
			SetLimiters(xtplPrinter,'y', 0, 1, 0, 0, 1, 0);
			break;
		case 'x':
			// X
			SetTiming(xtplPrinter,'x', 20000);
			SetMaxPosition(xtplPrinter,'x', 10000000);
			SetCurrPosition(xtplPrinter,'x', 5000000);
			SetLimiters(xtplPrinter,'x', 0, 0, 0, 0, 1, 0);
			break;
		case 'z':
			//Z
			SetTiming(xtplPrinter,'z', 20000);
			SetMaxPosition(xtplPrinter,'z', 10000000);
			SetCurrPosition(xtplPrinter,'z', 5000000);
			SetLimiters(xtplPrinter,'z', 0, 0, 0, 0, 1, 0);
			break;
		case 'a':
			//A
			SetTiming(xtplPrinter,'a', 20000);
			SetMaxPosition(xtplPrinter,'a', 10000000);
			SetCurrPosition(xtplPrinter,'a', 5000000);
			SetLimiters(xtplPrinter,'a', 0, 0, 0, 0, 1, 0);
			break;
		}
		break;

	default:
		// Y
		SetTiming(xtplPrinter,'y', 20000);
		SetMaxPosition(xtplPrinter,'y', 1000000);
		SetCurrPosition(xtplPrinter,'y', 20000);
		SetLimiters(xtplPrinter,'y', 0, 1, 0, 0, 1, 0);
		// X
		SetTiming(xtplPrinter,'x', 20000);
		SetMaxPosition(xtplPrinter,'x', 1000000);
		SetCurrPosition(xtplPrinter,'x', 20000);
		SetLimiters(xtplPrinter,'x', 0, 0, 0, 0, 1, 0);
		//Z
		SetTiming(xtplPrinter,'z', 20000);
		SetMaxPosition(xtplPrinter,'z', 1000000);
		SetCurrPosition(xtplPrinter,'z', 20000);
		SetLimiters(xtplPrinter,'z', 0, 0, 0, 0, 1, 0);
		//A
		SetTiming(xtplPrinter,'a', 20000);
		SetMaxPosition(xtplPrinter,'a', 450000);
		SetCurrPosition(xtplPrinter,'a', 20000);
		SetLimiters(xtplPrinter,'a', 0, 0, 0, 0, 1, 0);
		break;
	}
}

// ruch do pozycji 
void Move(cmdHandler*xtplPrinter, char axis, int dest, int speed) {
	switch (axis) {
	//std::cout << xpos << " " << ypos << " " << zpos << " " << apos << std::endl;
	case 'x':
		xpos = dest;
		break;
	case 'y':
		ypos = dest;
		break;
	case 'z':
		zpos = dest;
		break;
	case 'a':
		apos = dest;
		break;
	}
	char* mask = AxisToMask(xtplPrinter, axis);
	char destch[10];
	itoa(dest, destch, 10);
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_set_dst_position ";
	SetTiming(xtplPrinter, axis, speed);
	cmd.append(mask);
	cmd.append(" ");
	cmd.append(destch);
	cmd.append(" 1");
	//std::cout << cmd << std::endl;
	std::cout << "Pos X: " << xpos << " Pos Y: " << ypos << " Pos Z: " << zpos << " Pos A: " << apos << std::endl;
	xtplPrinter->sendCmd((char*)cmd.c_str(),false);
	//Sleep(25);
}

// ustawienie pozycji docelowej (ruch odbywa siê po smb_start_move)
void SetMove(cmdHandler*xtplPrinter, char axis, int dest, int speed) // ustawienie pozycji docelowej
{
	switch (axis) {
		//std::cout << xpos << " " << ypos << " " << zpos << " " << apos << std::endl;
	case 'x':
		xpos = dest;
		break;
	case 'y':
		ypos = dest;
		break;
	case 'z':
		zpos = dest;
		break;
	case 'a':
		apos = dest;
		break;
	}
	char* mask = AxisToMask(xtplPrinter, axis);
	char destch[10];
	itoa(dest, destch, 10);
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_set_dst_position ";
	SetTiming(xtplPrinter, axis, speed);
	cmd.append(mask);
	cmd.append(" ");
	cmd.append(destch);
	cmd.append(" 0");
	//std::cout << cmd << std::endl;
	std::cout << "Pos X: " << xpos << " Pos Y: " << ypos << " Pos Z: " << zpos << " Pos A: " << apos << std::endl;
	xtplPrinter->sendCmd((char*)cmd.c_str());
	//Sleep(25);
}

// rozpoczêcie ruchu do ustawionej pozycji
void smb_start_move(cmdHandler*xtplPrinter)
{
	char* mask = "0x0F";
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_start ";
	cmd.append(mask);
	xtplPrinter->sendCmd((char*)cmd.c_str());
}

// "awaryjne" zatrzymanie wszystkich osi
void smb_stop_move(cmdHandler*xtplPrinter)
{
	char* mask = "0x0F";
	std::string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_stop ";
	cmd.append(mask);
	xtplPrinter->sendCmd((char*)cmd.c_str());

	// Dodac czytanie pozycji i zapisanie w xpos ypos zpos
	// std::cout << "Pos X: " << xpos << " Pos Y: " << ypos << " Pos Z: " << zpos << " Pos A: " << apos << std::endl;
}