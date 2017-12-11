
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
#include "adcTest.h"
#include "Process.h"
#include "pulseSignalHandler.h"

extern bool manualgui;
extern bool advancedcontrol;
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
extern int contact_z_pos;
signalParameters inParams = { 0,0,0,1,20,1,9 };
signalRes outParam;
pulseParams PulseinParams;
pulseRes PulseoutParams;





using namespace std;

void AdvancedControl(cmdHandler* xtplPrinter)
{
	int StartPad, EndPad, UserVac, UserVdc, UserFreq;

	ifstream menuAdvCtrl;
	menuAdvCtrl.open("menuAdvancedControl.txt");
	cout << menuAdvCtrl.rdbuf() << endl;
	menuAdvCtrl.close();

	while (advancedcontrol) 
	{
		switch (getch()) {
			//   ------------------ EXIT ----------------------
			case 27: // ESC
				smb_stop_move(xtplPrinter);
				advancedcontrol = false;
				cout << "Exiting manual movement menu.\n";
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
				break;

			case 13:  // STATUS - ENTER
				cout << "POSITION\nxpos = " << xpos << " ypos = " << ypos << " zpos = " << zpos << endl;
				cout << "SIGNAL\n" << "Vac = " << Vac << " Vdc = " << Vdc << " freq = " << freq << endl;
				break;

			case 't':
				PrintProcess(xtplPrinter, -12820 * 4, 250, 5000000000);
				// 400 * 64 = 1 mm
				break;

			case 'y':
				//druk automatyczny jeden pad
				Move(xtplPrinter, 'x', 485970, 300000);
				break;

			case 'g':
				Move(xtplPrinter, 'x', 1822000, 6000);
				getch();
				Move(xtplPrinter, 'y', 274000, 10000);
				getch();
				Move(xtplPrinter, 'z', 350000, 8000);
				break;

			case 'h':
				//pozycja druku pierwszy pad
				Move(xtplPrinter, 'x', 469000, 6000);
				getch();
				Move(xtplPrinter, 'y', 113000, 10000);
				getch();
				Move(xtplPrinter, 'z', 446000, 8000);
				break;

			case 'z':
				Move(xtplPrinter, 'z', 30000, 10000);
				Sleep(500);
				break;

			case 'r':
				SetRelayManual(xtplPrinter, 5);
				break;

			case 'c':
				ContactCheck(xtplPrinter, true, 1);
				break;


			case 'v':
				cout << "Witamy w automatycznym procesie dozowania XTPL. Prosze ustawic pozycje pocz¹tkowa dozowania, a nastêpnie nacisnac spacje. Wcisniecie innego przycisku spowoduje powrot do menu recznego sterowania" << endl;
				Sleep(10);
				switch (getch())
				{
				case ' ':
					cout << "Loading start dispensing position" << endl;
					fileToPosAutoDispensing(xtplPrinter);
					AutoDispensing(xtplPrinter, xpos, ypos, zpos, 1, 8); //wybór padów
					cout << "Dispensing finished. Returend to manual movement menu." << endl;
					break;
				default:
					break;
				}
				break;

			case 'b':
				cout << "Witamy w automatycznym procesie drukowania XTPL. Prosze ustawic pozycje pocz¹tkowa dozowania, a nastêpnie nacisnac spacje. Wcisniecie innego przycisku spowoduje powrot do menu recznego sterowania" << endl;
				switch (getch())
				{
				case ' ':
					cout << "Loading start position" << endl;
					fileToPosAutoPrinting(xtplPrinter);
					cout << "Start pad: " << endl;
					cin >> StartPad;
					cout << "End pad: " << endl;
					cin >> EndPad;
					cout << "Vac: " << endl;
					cin >> inParams.amplitude;
					cout << "Vdc: " << endl;
					cin >> inParams.dcOffsetValue;
					cout << "Freq: " << endl;
					cin >> inParams.freqDiv;
					AutoPrinting(xtplPrinter, xpos, ypos, zpos, StartPad, EndPad, inParams.amplitude, inParams.dcOffsetValue, inParams.freqDiv);
					break;
				default:
					break;
				}
				break;


		}
	}
}


void PrintProcess(cmdHandler* xtplPrinter, int printX, int printA, long long int printtime)  //printX - 12820 steps = 1 mm , printA - 12820 steps = 1 mm,  printtime - ns)
{
	int timingX = 10000;
	int timingA = 10000;
	xpos = xpos + printX;
	apos = apos + printA;
	printX = abs(printX);
	printA = abs(printA);
	timingX = (printtime/printX - 420)/10;
	timingA = (printtime/printA - 420)/10;
	
	if (timingX>6000 && timingA>6000 && apos<450000)
	{
		SetMove(xtplPrinter, 'x', xpos, timingX);
		SetMove(xtplPrinter, 'a', apos, timingA);
		cout << "timX = " << timingX << endl;
		cout << "timA = " << timingA << endl;
		cout << "Process in progress..." << endl;
		smb_start_move(xtplPrinter);
	}
	else
	{
		if (apos>=450000)
			cout << "A limit" << endl;
		else
			cout << "Speed limit - increase the time or decrease the total length of printing" << endl;
		cout << "apos = " << apos << endl;
		cout << "timX = " << timingX  << endl;
		cout << "timA = " << timingA << endl;
	}
}

bool ZAdjust(cmdHandler*xtplPrinter, int safeZ, int relay)
{
	cout << "ZAdjust..." << endl;
	// skan kontaktu
	while (1)
	{
		if (ContactCheck(xtplPrinter, true, relay) == true)
		{
			contact_z_pos = zpos;
			Move(xtplPrinter, 'z', zpos - 256, 10000); // ruch w gore o 256 kroków =~ 20 um
			cout << "Contact position: " << contact_z_pos << endl;
			break;
		}
		else if (zpos>=safeZ)
		{
			cout << "WARNUNG! Safe limit reached." << endl;
			smb_stop_move(xtplPrinter);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x01", false);
			ypos = xtplPrinter->getPosResValue(1);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x02", false);
			xpos = xtplPrinter->getPosResValue(1);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x04", false);
			zpos = xtplPrinter->getPosResValue(1);
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x08", false);
			apos = xtplPrinter->getPosResValue(1);
			cout << "POSITION\nxpos = " << xpos << " ypos = " << ypos << " zpos = " << zpos << endl;
			cout << "Exiting auto-mode.\n";
			return 0;
			break;
		}
		Move(xtplPrinter, 'z', zpos + 64, 6000); // ruch w dol o 64 kroków =~ 5 um

		Sleep(50);
	}
	return 1;
}

void AutoDispensing(cmdHandler*xtplPrinter, int xStartPos, int yStartPos, int zStartPos, int StartingPad, int EndingPad)
{
	Move(xtplPrinter, 'x', xStartPos, 6000);
	if (SleepOrSkip(xtplPrinter, 600) == false) return;
	Move(xtplPrinter, 'y', yStartPos + (StartingPad - 1) * 10204, 6000);
	if (SleepOrSkip(xtplPrinter, 600) == false) return;
	Move(xtplPrinter, 'z', zStartPos, 6000);
	if (SleepOrSkip(xtplPrinter, 600) == false) return;

	SetRelayAuto(xtplPrinter, StartingPad, 5);
	if (ZAdjust(xtplPrinter, 569000, StartingPad) == false)
	{
		cout << "Safe limit reached" << endl;
		return;
	}
	Move(xtplPrinter, 'z', contact_z_pos - 256, 10000);
	Sleep(1000);

	for (StartingPad; StartingPad <= EndingPad; StartingPad++)
	{
		Move(xtplPrinter, 'x', xStartPos, 6000);
		SetRelayAuto(xtplPrinter, StartingPad, 5);
		Move(xtplPrinter, 'y', yStartPos + (StartingPad - 1) * 10204, 6000); // Y 2mm forward
		if (SleepOrSkip(xtplPrinter, 60) == false) return;
		Move(xtplPrinter, 'z', contact_z_pos, 10000); // 1mm down
		if (SleepOrSkip(xtplPrinter, 60) == false) return;
		if (ContactCheck(xtplPrinter, true, StartingPad) == false)
		{
			Move(xtplPrinter, 'z', contact_z_pos - 6000, 10000);
			Sleep(3000);
			if (ZAdjust(xtplPrinter, 569000, StartingPad) == false)
			{
				cout << "Safe limit reached" << endl;
				return;
			}
			Sleep(1000);
		}
		cout << "Z poprawne" << endl;
		Move(xtplPrinter, 'z', contact_z_pos - 256, 10000); // 20 um up
		Sleep(1000);
		Move(xtplPrinter, 'a', apos + 50, 6000); // dispensing drop
		Sleep(2000);
		//Move(xtplPrinter, 'z', contact_z_pos - 6000, 10000); // up
		//Sleep(2000);
		Move(xtplPrinter, 'x', xpos - 5000, 6000); // step left
		Sleep(2000);
		//Move(xtplPrinter, 'z', contact_z_pos - 100, 10000); // 10 um up
		//Sleep(2000);
		Move(xtplPrinter, 'y', ypos - 1000, 20000); // blur drop 
		Sleep(2000);
		Move(xtplPrinter, 'y', ypos + 2000, 20000); // blur drop
		Sleep(2000);
		Move(xtplPrinter, 'y', ypos - 1000, 20000); // blur drop
		Sleep(2000);
		PrintProcess(xtplPrinter, -12820 * 4.5, 800, 20000000000); //dispensing - .. s
		if (SleepOrSkip(xtplPrinter, 2000) == false) return;
		//Move(xtplPrinter, 'y', ypos - 1000, 20000); // blur drop 
		//Sleep(1000);
		//Move(xtplPrinter, 'y', ypos + 2000, 20000); // blur drop
		//Sleep(1000);
		//Move(xtplPrinter, 'y', ypos - 1000, 20000); // blur drop
		//Sleep(1000);
		Move(xtplPrinter, 'z', zStartPos - 12820, 10000); // 1mm up
		Sleep(1000);
	}
	Move(xtplPrinter, 'z', zStartPos - 2 * 12820, 10000); // 2mm up
	Sleep(1000);
	Move(xtplPrinter, 'z', 30000, 6000);
	if (SleepOrSkip(xtplPrinter, 100) == false) return;

}

void AutoPrinting(cmdHandler*xtplPrinter, int xStartPos, int yStartPos, int zStartPos, int StartingPad ,int EndingPad, int Vac, int Vdc, int freq)
{
	
	int counter = 0;
	PulseinParams.dac0val = 5000;
	PulseinParams.dac1val = 0;
	PulseinParams.duration = 10;
	PulseinParams.lineresistance = LINE_RES_5K;

	// Pozycja startowa x i y musi znajdowaæ siê nad starting padem. Pozycja startowa Z powinna byæ kilka mm nad padem.
	// Movement to start position
	Move(xtplPrinter, 'x', xStartPos, 6000);
	if (SleepOrSkip(xtplPrinter, 600) == false) return;
	Move(xtplPrinter, 'y', yStartPos + (StartingPad - 1) * 10204, 6000);
	if (SleepOrSkip(xtplPrinter, 600) == false) return;
	Move(xtplPrinter, 'z', zStartPos, 6000);
	if (SleepOrSkip(xtplPrinter, 600) == false) return;

	// starting pad relay on + 5k resistor
	SetRelayAuto(xtplPrinter, StartingPad, 5);

	//Scanning for contact
	if (ZAdjust(xtplPrinter, 503600, StartingPad) == false)
	{
		cout << "Safe limit reached" << endl;
		return;
	}
	Move(xtplPrinter, 'z', contact_z_pos, 10000);
	Sleep(1000);
	
	//Printing Process
	for (StartingPad; StartingPad <= EndingPad; StartingPad++)
	{
		
		Move(xtplPrinter, 'x', xStartPos, 6000);
		SetRelayAuto(xtplPrinter, StartingPad, 5);
		Move(xtplPrinter, 'y', yStartPos + (StartingPad - 1) * 10204, 6000); // Y 2mm forward
		if (SleepOrSkip(xtplPrinter, 60) == false) return;
		Move(xtplPrinter, 'z', contact_z_pos, 10000); // 1mm down
		
		if (SleepOrSkip(xtplPrinter, 60) == false) return;
		if (ContactCheck(xtplPrinter, true, StartingPad) == false)
		{
			Move(xtplPrinter, 'z', contact_z_pos - 6000, 10000);
			Sleep(3000);
			if (ZAdjust(xtplPrinter, 503600, StartingPad) == false)
			{
				cout << "Safe limit reached" << endl;
				return;
			}
			Sleep(1000);
			Move(xtplPrinter, 'z', contact_z_pos, 10000);
		}
		cout << "Z ok" << endl;
		if (SleepOrSkip(xtplPrinter, 30) == false) return;
		cout << "Scanning for pad end:" << endl;
		while (1)
		{
			Move(xtplPrinter, 'x', xpos - 50, 30000);
			if (counter == 3)
			{
				counter = 0;
				cout << "Process start. X pos:" << endl;
				smb_stop_move(xtplPrinter);
				cout << xpos << endl;
				break;
			}
			if (ContactCheck(xtplPrinter, false, StartingPad) == false) counter++;
			Sleep(50);
		}
		Move(xtplPrinter, 'x', xpos - 3.5 * 12820, 300000); // printing
		
		Sleep(5);
		cout << "signal set" << endl;
		inParams.amplitude = Vac;
		inParams.dcOffsetValue = Vdc;
		inParams.freqDiv = freq;
		setSignalParameters(xtplPrinter, inParams);
		setSignalForm(xtplPrinter, sinus);
		Sleep(5);
		startSignalGeneration(xtplPrinter);
		cout << "signal start" << endl;
		if (SleepOrSkip(xtplPrinter, 1800) == false) return;

		Softoff(xtplPrinter, 5, 5000, 5000, 5000);
		stopSignalGeneration(xtplPrinter);
		PulseinParams.relaySelection = RelayMask(xtplPrinter, StartingPad);
		generatePulse(xtplPrinter, PulseinParams, &PulseoutParams);
		cout << "Avg. current: " << PulseoutParams.avgCurrent << endl;
		cout << "Line resistance: " << PulseoutParams.calcResistance << endl;

		cout << "Printing on this pad has been finished. Tap any key to go to the next pad." << endl;
		getch();

		Move(xtplPrinter, 'z', contact_z_pos - 12820, 10000); // 1mm up
		

		Sleep(1000);
	}
	Move(xtplPrinter, 'z', 60000, 6000);
	if (SleepOrSkip(xtplPrinter, 30) == false) return;
	cout << "Printing finished" << endl;
}

int SleepOrSkip(cmdHandler *xtplPrinter, int ms100)
{
	bool AutoMode = true;
	cout << "Tap space key to skip. Tap ESC to stop and exit automode." << endl;
	while (ms100 >= 0 && AutoMode == true)
	{
		if (kbhit())
		{
			char c = getch();
			switch (c)
			{
			case ' ':
				return 1;
				break;
			case 27: // ESC
				smb_stop_move(xtplPrinter);
				xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x01", false);
				ypos = xtplPrinter->getPosResValue(1);
				xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x02", false);
				xpos = xtplPrinter->getPosResValue(1);
				xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x04", false);
				zpos = xtplPrinter->getPosResValue(1);
				xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_get_status 0x08", false);
				apos = xtplPrinter->getPosResValue(1);
				cout << "POSITION\nxpos = " << xpos << " ypos = " << ypos << " zpos = " << zpos << endl;
				AutoMode = false;
				cout << "Exiting auto-mode.\n";
				return 0;
				break;
			}


		}
		Sleep(100);
		ms100--;
	}
	return 1;
}

void Softoff(cmdHandler *xtplPrinter, float Vacdecr, int SlopeTime1, int WaitTime, int SlopeTime2)
{
	
	float Vacdecrstep1 = (inParams.amplitude - Vacdecr) / 500;
	double Vacdecrstep2 = Vacdecr / 500;
	float Vdcdecrstep2 = inParams.dcOffsetValue / 500;
	float stepac, stepdc;
	setSignalParameters(xtplPrinter, inParams);
	setSignalForm(xtplPrinter, sinus);
	startSignalGeneration(xtplPrinter);
	if (SlopeTime1 < 0) SlopeTime1 = 0;
	if (SlopeTime2 < 0) SlopeTime2 = 0;

	cout << "First slope begin: " << endl;
	for (int i = 1; i < 501; i++)	// 1st slope
	{
		inParams.amplitude = inParams.amplitude - Vacdecrstep1;
		inParams.dcOffsetValue = inParams.dcOffsetValue - Vdcdecrstep2;
		if (inParams.dcOffsetValue < 0) inParams.dcOffsetValue = 0;
		if (inParams.amplitude < 0) inParams.amplitude = 0;
		setSignalParameters(xtplPrinter, inParams);
		Sleep(SlopeTime1 / 500);
	}

	Sleep(WaitTime); // wait time
	cout << "Second slope..." << endl;
	for (int j = 1; j < 501; j++)	// 2nd slope
	{
		inParams.amplitude = inParams.amplitude - Vacdecrstep2;
		inParams.dcOffsetValue = inParams.dcOffsetValue - Vdcdecrstep2;
		if (inParams.dcOffsetValue < 0) inParams.dcOffsetValue = 0;
		if (inParams.amplitude < 0) inParams.amplitude = 0;
		setSignalParameters(xtplPrinter, inParams);
		Sleep(SlopeTime2 / 500);

	}
	cout << "SoftOff has ended: " << endl;
}

int RelayMask(cmdHandler *xtplPrinter, int relay)
{
	switch (relay) {
	case 1:
		return RELAY_1;
		break;
	case 2:
		return RELAY_2;
		break;
	case 3:
		return RELAY_3;
		break;
	case 4:
		return RELAY_4;
		break;
	case 5:
		return RELAY_5;
		break;
	case 6:
		return RELAY_6;
		break;
	case 7:
		return RELAY_7;
		break;
	case 8:
		return RELAY_8;
		break;
	default:
		return 0;
		break;
	}
}

bool ContactCheck(cmdHandler*xtplPrinter, bool resp, int relay)
{
	pulseParams inParams;
	pulseRes response = { 0,0,0 };

	inParams.dac0val = 0;
	inParams.dac1val = 5000;
	inParams.duration = 1;
	inParams.lineresistance = LINE_RES_5K;
	inParams.relaySelection = RelayMask(xtplPrinter, relay);
	generatePulse(xtplPrinter, inParams, &response);

	cout << "Average Current: " << response.avgCurrent << endl;
	cout << "Calculated Resistance: " << response.calcResistance << endl;

	if (response.avgCurrent > 670 || response.avgCurrent < -670) // prad > 670 uA, czyli R < ~1kohm
	{
		if (resp == 1) cout << "ZWARCIE!!!" << endl;
		return true;
	}
	else return false;
}