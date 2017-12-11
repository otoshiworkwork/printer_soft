#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <conio.h>
#include "dacTest.h"
bool sinus = 1;
int pulse_time = 1;
int pulse_U = 1000;
float time = 2.0;

using namespace std;

int SetSignal(cmdHandler*xtplPrinter, int Vac, int Vdc)
{
	char AC[10];
	itoa(Vac, AC, 10);
	char DC[10];
	//char buff[100];
	string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --signal_user ";
	if (sinus)
	{
		cmd.append(" 1 ");
	}
	else
	{
		cmd.append(" 1 ");
	}
	cmd.append(AC);
	cmd.append(" 0 65532");

	if (Vdc>0)
	{
		cmd.append(" 0");
	}
	else
	{
		cmd.append(" 1");
		Vdc = -Vdc;
	}
	itoa(Vdc, DC, 10);
	cmd.append(" ");
	cmd.append(DC);
	xtplPrinter->sendCmd((char*)cmd.c_str());
	cout << "Signal parameters: " << Vac << " " << Vdc << endl;
	
	return 0;
}

int SetFreq(cmdHandler*xtplPrinter, int ST)
{
	char st[10];
	itoa(ST, st, 10);
	string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --signal_time ";
	cmd.append(st);
	xtplPrinter->sendCmd((char*)cmd.c_str());
	return 0;
}

void SetRelayManual(cmdHandler*xtplPrinter, int ResistorValue)
{
	string RelayMask;
	string ResistorMask;
	cout << "Set relay (1-9): " << endl;
	switch (getch()) {
		case '1':
			RelayMask = "0x7F";
			break;
		case '2':
			RelayMask = "0xBF";
			break;
		case '3':
			RelayMask = "0xDF";
			break;
		case '4':
			RelayMask = "0xEF";
			break;
		case '5':
			RelayMask = "0xF7";
			break;
		case '6':
			RelayMask = "0xFB";
			break;
		case '7':
			RelayMask = "0xFD";
			break;
		case '8':
			RelayMask = "0xFE";
			break;
		case '0':
			RelayMask = "0xFF";
		default:
			return;
	}
	string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --digital_out_ctrl ";
	cmd.append(RelayMask);
	cmd.append(" ");

	switch (ResistorValue)
	{
	case 0:
		ResistorMask = "0x03";
		break;
	case 5:
		ResistorMask = "0x01";
		break;
	case 50:
		ResistorMask = "0x02";
		break;
	case 55:
		ResistorMask = "0x00";
		break;
	default:
		break;
	}
	cmd.append(ResistorMask);
	cout << cmd << endl;
	xtplPrinter->sendCmd((char*)cmd.c_str(), true);
	return;
}

void SetRelayAuto(cmdHandler*xtplPrinter, int RelayNumber, int ResistorValue)
{
	string RelayMask;
	string ResistorMask;
	switch (RelayNumber) {
	case 1:
		RelayMask = "0x7F";
		break;
	case 2:
		RelayMask = "0xBF";
		break;
	case 3:
		RelayMask = "0xDF";
		break;
	case 4:
		RelayMask = "0xEF";
		break;
	case 5:
		RelayMask = "0xF7";
		break;
	case 6:
		RelayMask = "0xFB";
		break;
	case 7:
		RelayMask = "0xFD";
		break;
	case 8:
		RelayMask = "0xFE";
		break;
	case 0:
		RelayMask = "0xFF";
	default:
		return;
	}

	string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --digital_out_ctrl ";
	cmd.append(RelayMask);
	cmd.append(" ");
	
	switch (ResistorValue)
	{
	case 0:
		ResistorMask = "0x03";
		break;
	case 5:
		ResistorMask = "0x01";
		break;
	case 50:
		ResistorMask = "0x02";
		break;
	case 55:
		ResistorMask = "0x00";
		break;
	default:
		break;
	}
	cmd.append(ResistorMask);
	cout << cmd << endl;
	xtplPrinter->sendCmd((char*)cmd.c_str(), true);
	return;
}

void SetPulse(cmdHandler*xtplPrinter, int pulse_U, int pulse_time)
{
	char time[10];
	char U[10];
	itoa(pulse_time, time, 10);
	itoa(pulse_U, U, 10);
	string cmd = "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --pulse_A 1 ";
	cmd.append(U);
	cmd.append(" 0 ");
	cmd.append(time);
	cout << cmd << endl;
	xtplPrinter->sendCmd((char*)cmd.c_str());
	xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_A 0");	
}

