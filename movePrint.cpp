
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <conio.h>
#include "moveGeneral.h"
#include "moveHoming.h"
#include "movePrint.h"
#include "posToFile.h"
#include "variables.h"
#include "dacTest.h"
#include "Process.h"
#include "adcTest.h"
#include "pulseSignalHandler.h"

extern bool manualgui;
extern int axispos;
extern int xpos;
extern int ypos;
extern int zpos;
extern int apos;
extern char axis;
extern int movestep;
extern int dispstep;
extern float Vac;
extern float Vdc;
extern float Vacstep;
extern float Vdcstep;
extern int freq;
extern int freqstep;

using namespace std;

void movePrint(cmdHandler*xtplPrinter) {
	ifstream menuMove;
	menuMove.open("menuMove.txt");
	cout << menuMove.rdbuf() << endl;
	menuMove.close();
	movestep = 1000;
	dispstep = 50;

	pulseParams inParams;
	pulseRes response = { 0,0,0 };
	inParams.dac0val = 5000;
	inParams.dac1val = 0;
	inParams.duration = 1;
	inParams.lineresistance = LINE_RES_5K;

	signalRes signalResults = { 0,0,0,0,0,0,0 };
	signalParameters signalInputParameters;
	signalInputParameters.amplitude = 25;
	signalInputParameters.freqDiv = 9;
	signalInputParameters.freqMul = 1;
	signalInputParameters.dcOffsetChannel = 0;
	signalInputParameters.dcOffsetValue = 0;
	signalInputParameters.minValLimiter = 0;
	signalInputParameters.maxValLimiter = 0;
	setSignalForm(xtplPrinter, sinus);
	setSignalParameters(xtplPrinter, signalInputParameters);
	stopSignalGeneration(xtplPrinter);

	while (manualgui == true)
	{
		switch (getch()) {
			// ------------------ movement XYZ ----------------------
		case 'd':
			Move(xtplPrinter, 'x', xpos + movestep, 10000);
			break;
		case 'a':
			Move(xtplPrinter, 'x', xpos - movestep, 10000);
			break;
		case 's':
			Move(xtplPrinter, 'y', ypos + movestep, 10000);
			break;
		case 'w':
			Move(xtplPrinter, 'y', ypos - movestep, 10000);
			break;
		case 'c':
			Move(xtplPrinter, 'z', zpos + movestep, 10000);
			break;
		case 'f':
			Move(xtplPrinter, 'z', zpos - movestep, 10000);
			break;

			// ------------------ movement XYZ ----------------------
		case '=':
			movestep = movestep + 500;
			if (movestep > 5000)
			{
				movestep = 5000;
				cout << "movestep MIN LIMIT" << endl;
			}
			cout << "movestep value = " << movestep << endl;
			break;
		case '-':
			movestep = movestep - 500;
			if (movestep < 0)
			{
				movestep = 0;
				cout << "movestep MIN LIMIT" << endl;
			}
			cout << "movestep value = " << movestep << endl;
			break;

			// ------------------ dispenser ----------------------
		case 'e':
			Move(xtplPrinter, 'a', apos + dispstep, 15000);
			break;
		case 'q':
			Move(xtplPrinter, 'a', apos - dispstep, 15000);
			break;
			//   ------------------ stop movement ----------------------
		case ' ':
			smb_stop_move(xtplPrinter);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x01", true);
			ypos = xtplPrinter->getPosResValue(1);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x02", true);
			xpos = xtplPrinter->getPosResValue(1);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x04", true);
			zpos = xtplPrinter->getPosResValue(1);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x08", true);
			apos = xtplPrinter->getPosResValue(1);
			cout << "POSITION\nxpos = " << xpos << " ypos = " << ypos << " zpos = " << zpos << endl;
			//manualgui = false;
			//cout << "Exiting manual movement menu.\n";
			break;
			//   ------------------ EXIT ----------------------
		case 27: // ESC
			smb_stop_move(xtplPrinter);
			manualgui = false;
			cout << "Exiting manual movement menu.\n";
			break;
			// ---------------------- signalInputParameters.amplitude ---------------------------
		case 'u':   // -Vac
			signalInputParameters.amplitude = signalInputParameters.amplitude - Vacstep;
			setSignalParameters(xtplPrinter, signalInputParameters);
			cout << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
		case 'i':   // +Vac
			signalInputParameters.amplitude = signalInputParameters.amplitude + Vacstep;
			setSignalParameters(xtplPrinter, signalInputParameters);
			cout << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
			// ---------------------- signalInputParameters.dcOffsetValue ---------------------------
		case 'j':   // -Vdc
			signalInputParameters.dcOffsetValue = signalInputParameters.dcOffsetValue - Vdcstep;
			setSignalParameters(xtplPrinter, signalInputParameters);
			cout << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
		case 'k':   // +Vdc
			signalInputParameters.dcOffsetValue = signalInputParameters.dcOffsetValue + Vdcstep;
			setSignalParameters(xtplPrinter, signalInputParameters);
			cout << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
		case 'l':   // kierunek DC
			if (signalInputParameters.dcOffsetChannel == 1) signalInputParameters.dcOffsetChannel = 0;
			else signalInputParameters.dcOffsetChannel = 1;
			setSignalParameters(xtplPrinter, signalInputParameters);
			cout << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
			// ---------------------- freq ---------------------------
		case 'n':  //  freq--
			freq = freq + 1;
			signalInputParameters.freqDiv = freq;
			setSignalParameters(xtplPrinter, signalInputParameters);
			cout << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
		case 'm':  // freq ++
			freq = freq - 1;
			signalInputParameters.freqDiv = freq;
			setSignalParameters(xtplPrinter, signalInputParameters);
			cout << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
			// ---------------------- Signal ON/OFF ---------------------------
		case 'p':  // ON
		{
			setSignalForm(xtplPrinter, sinus);
			setSignalParameters(xtplPrinter, signalInputParameters);
			startSignalGeneration(xtplPrinter);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --signal_start", true);
			cout << "SIGNAL ON\n" << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
		}
		case 'o':  // OFF
		{
			setSignalForm(xtplPrinter, sinus);
			setSignalParameters(xtplPrinter, signalInputParameters);
			stopSignalGeneration(xtplPrinter);
			cout << "SIGNAL OFF\n" << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;
		}
		case 'z':
		{
			stopSignalGeneration(xtplPrinter);
			ContactCheck(xtplPrinter, true, 0x00);
			break;
		}
			// ---------------------- POS/SIGNAL STATUS ---------------------------
		case 13:  // STATUS - ENTER
			cout << "POSITION\nxpos = " << xpos << " ypos = " << ypos << " zpos = " << zpos << endl;
			cout << "SIGNAL\n" << "signalInputParameters.amplitude = " << signalInputParameters.amplitude << " signalInputParameters.dcOffsetValue = " << signalInputParameters.dcOffsetValue << " freq = " << freq << endl;
			break;

		default:
			cout << "default" << endl;
			break;
		
		}
	}
	cout << "Position saved to file..\n";
	posToFile(xtplPrinter);
}