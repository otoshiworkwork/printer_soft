#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <conio.h>
#include "adcTest.h"
#include "pulseSignalHandler.h"

using namespace std;


void ClearBuff(cmdHandler*xtplPrinter)
{
	ofstream ADC1_DATA;
	ADC1_DATA.open("ADC1.txt");
	ADC1_DATA.close();
}


void GetCurrentFromPulse(cmdHandler* xtplPrinter, int PulseVal, int PulseDur)
{
	ofstream ADC1_DATA;
	double Vdiff = 0;
	for (int i = 0; i<1; i++) 
	{
		char PulseValCh[10];
		itoa(PulseVal, PulseValCh, 10);
		char PulseDurCh[10];
		itoa(PulseDur, PulseDurCh, 10);
		string cmd = "ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_a 1 0 ";
		cmd.append(PulseValCh);
		cmd.append(" ");
		cmd.append(PulseDurCh);
		xtplPrinter->sendCmd((char*)cmd.c_str());
		//cout << "Pulse parameters: " << PulseValCh << " " << PulseDurCh << endl;

		while (1) {
			xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_a 0 1 1", false);

			if (POSITIVE_RES == xtplPrinter->getStatus()) {
				if (0 == xtplPrinter->getPosResValue(0)) {
					break;
				}
			}
			cout << ".";
			Sleep(10);
		}
		cout << endl;
		xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_a 0 1 1", false);

		ADC1_DATA.open("ADC1.txt", ios::out | ios::app);
		Vdiff = (xtplPrinter->getPosResValue(2) - xtplPrinter->getPosResValue(1));
		//ADC1_DATA << PulseValCh << " \t" << xtplPrinter->getPosResValue(4) << " \t" << xtplPrinter->getPosResValue(5) << " \t" << Vdiff << endl;
		ADC1_DATA << PulseValCh << " \t" << Vdiff << endl;
		cout << PulseValCh << " \t" << (xtplPrinter->getPosResValue(2)) << " \t" << (xtplPrinter->getPosResValue(1)) << " \t" << Vdiff << endl;
		
		//cout << Vdiff << " uA" << endl;
		ADC1_DATA.close();
		
		Sleep(5);
	}

}


