/*
 *  main.cpp
 *
*/
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include "cmdHandler.h"
#include "peltierTest.h"
#include "dacTest.h"
#include "peltierUser.h"
#include "moveGeneral.h"
#include "moveHoming.h"
#include "movePrint.h"
#include "posToFile.h"
#include "Process.h"
#include "pulseSignalHandler.h"

bool online = true;
extern bool fileload = true;
extern bool TECcfg = true;
extern bool manualgui = true;
extern bool advancedcontrol = true;
extern int xpos = 300000;
extern int ypos = 300000;
extern int zpos = 300000;
extern int apos = 300000;
extern char axis;
extern int movestep = 1000;
extern int dispstep = 50;
extern float Vac = 10;
extern float Vdc = 1;
extern float Vacstep = 1;
extern float Vdcstep = 0.5;
extern int freq = 9;
extern int freqstep = 1;
extern int contact_z_pos = 300000;

using namespace std;

/* User code begin */
/* Paths for developing mode*/
char releasePath[1000] = "..\\__release_20171120_1608";
char deviceName[100]   = "ics32sx";
char close = 1;
/* User code end */
///////////////////////////////////////////////////////////////////////////////
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	if (CTRL_CLOSE_EVENT == dwCtrlType) {
		close = 0;
		return TRUE;
	}
	return FALSE;
}

int main(int argc, char* argv[]){


  if(argc > 1) {
    /* User put paths in stdin */
    strcpy_s(releasePath, argv[1]);
    strcpy_s(deviceName,  argv[2]);
  }

  /* XTPL_PRINTER_VERIFICATION BEGIN*/
  char retStat = NEGATIVE_RES;
  cmdHandler* xtplPrinter = new cmdHandler(releasePath, deviceName);
  initAmplifiersBoard(xtplPrinter);

  /* User code begin */
  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_stop 0x0f");  
  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 1 0");
  MoveInit(xtplPrinter, 'x', 1);
  MoveInit(xtplPrinter, 'y', 1);
  MoveInit(xtplPrinter, 'z', 1);
  MoveInit(xtplPrinter, 'a', 1);

 


  Sleep(500);
  
  while (online == 1)
  {
	  Sleep(500);
	  ifstream menuMain;
	  ifstream menuFile;
	  menuMain.open("menuMain.txt");
	  cout << menuMain.rdbuf() << endl;
	  menuMain.close();
	  switch (getch()) {
	  case '0':
		  cout << "Are you sure you want to home all axis? Press 'y' for yes, any other button for no\n";
		  switch (getch()) 
		  {
		  case 'y':
			  cout << "[Base all axis...]\n";
			  moveHoming(xtplPrinter, 'z');
			  Sleep(500);
			  moveHoming(xtplPrinter, 'y');
			  Sleep(500);
			  moveHoming(xtplPrinter, 'x');
			  Sleep(500);
			  moveHoming(xtplPrinter, 'a');
			  Sleep(500);
			  cout << "Homing done.\n";
			  break;
		  default:
			  break;
		  }
		  Sleep(500);
		  break;


	  case '1':
		  
		  break;

	  case '2':
		  cout << "[Advanced control...]\n";
		  advancedcontrol = true;
		  AdvancedControl(xtplPrinter);
		  break;

	  case '3':
		  cout << "[TEC Control...]\n";
		  TECcfg = true;
		  peltierUser(xtplPrinter);
		  break;

	  case '4': 
	  {
		  ofstream SinusResults;
		  signalRes signalResults = { 0,0,0,0,0,0,0 };
		  signalParameters signalInputParameters;
		  signalInputParameters.amplitude = 25;
		  signalInputParameters.freqDiv = 9;
		  signalInputParameters.freqMul = 1;
		  signalInputParameters.dcOffsetChannel = 1;
		  signalInputParameters.dcOffsetValue = 0;
		  signalInputParameters.minValLimiter = 0;
		  signalInputParameters.maxValLimiter = 0;
		  relayAndResistanceSelection(xtplPrinter, 0x00, LINE_RES_5K);
		  setSignalForm(xtplPrinter, sinus);
		  setSignalParameters(xtplPrinter, signalInputParameters);
		  startSignalGeneration(xtplPrinter);
		  SinusResults.open("SinusResults.txt");
		  SinusResults.close();
		  SinusResults.open("SinusResults.txt", ios::out | ios::app);
		  getSignalMeasResults(xtplPrinter, signalInputParameters, 100, LINE_RES_5K, &signalResults);
		  cout << "Vdrop: " << signalResults.avgDiffVoltage << endl;
		  cout << "avgCurrent: " << signalResults.avgCurrent << endl;
		  cout << "calcResistance: " << signalResults.calcResistance << endl;
		  stopSignalGeneration(xtplPrinter);
		  break;
		  /*for (int i = 0; i < 20; i++)
		  {
			  getch();
			  cout << i << ": " << endl;
			  getSignalMeasResults(xtplPrinter, signalInputParameters, 100, LINE_RES_5K, &signalResults);
			  cout << "Vdrop: " << signalResults.avgDiffVoltage << endl;
			  cout << "avgCurrent: " << signalResults.avgCurrent << endl;
			  cout << "calcResistance: " << signalResults.calcResistance << endl;
			  SinusResults << signalResults.avgCurrent << '\t' << signalResults.calcResistance << '\t' << endl;
		  }
		  SinusResults.close();
		  stopSignalGeneration(xtplPrinter);
		  break;*/
	  }
	  case '5':
		  manualgui = true;
		  cout << "[Manual control...]\n";
		  movePrint(xtplPrinter);
		  break;		  
	  case '6':
		  menuFile.open("menuFile.txt");
		  cout << menuFile.rdbuf() << endl;
		  menuFile.close();
		  switch (getch()) {
		  case 's':
			  cout << "Position saved to file.\n";
			  posToFile(xtplPrinter);
			  break;

		  case 'v':
			  cout << "Position saved to file.\n";
			  posToFileAutoDispensing(xtplPrinter);
			  break;

		  case 'b':
			  cout << "Position saved to file.\n";
			  posToFileAutoPrinting(xtplPrinter);
			  break;

		  case 'd':
			  cout << "Position loaded from file.\n";
			  fileToPos(xtplPrinter);
			  break;

		  case 'h':
			  cout << "Position set as home.\n";
			  posAsHome(xtplPrinter);
			  break;
		  case 'f':
			  moveToPos(xtplPrinter);
			  break;
		  default:
			  cout << "Leaving...\n";
			  break;
		  }
		  Sleep(500);
		  break;
	  case 'z':
		  cout << "Going offline...\n";
		  online = false; 
		  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --smb_stop 0x0f");
		  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 1 0");
		  Sleep(500);
		  break;

	  default:
		  break;
	  }

  }

  return 0;
}

