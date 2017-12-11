/*
 *  peltierTests.cpp
 *
 *  Created on: 18 oct 2017
 *      Author: PZ
 */
#include <iostream>
#include <fstream>
#include "peltierTest.h"

/* temperature Check test */
char temperatureCheck(cmdHandler*xtplPrinter) {
  char retStat = NEGATIVE_RES;

  retStat = xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 2 15000 50 3500");
  if(retStat == POSITIVE_RES) {
    for(int i =0; i< xtplPrinter->getPosResSize(); i++){
      std::cout << xtplPrinter->getPosResValue(i) << std::endl;
    }
  }else{
   for(int i =0; i< xtplPrinter->getNegResSize(); i++){
      std::cout << xtplPrinter->getNegResValue(i) << std::endl;
    }
  }

  return retStat;
}

/* temperature Profiler */
char temperatureProfiler(cmdHandler*xtplPrinter) {
  char retStat = NEGATIVE_RES;
  std::ofstream csvFile;

  csvFile.open("peltierTemperatureProfiler.csv");
  if(csvFile.good() != TRUE) {
    std::cout << "V#: Failed to open peltierTemperatureProfiler.csv" << std::endl;
    return CANNOT_OPEN_FILE;
  }
  /* Load PID coeffs to xtpl_peltier_ctrl*/
  retStat = xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 2 20000 50 4500");
  if(POSITIVE_RES == retStat) {
    csvFile << "PID Coeff:" << std::endl;
    for(int i = 0; i < xtplPrinter->getPosResSize(); i++) {
      csvFile << xtplPrinter->getPosResValue(i)<<';';
    }
    csvFile << std::endl << "Measurement:" << std::endl;;
  }

  //Set target table temperature to 1 degree
  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 1 1");

  //wait for temperature stabilization
  int delay = 0;
  while(delay != 600){ /*~1min*/
    // Delay 100ms
    Sleep(100); 
    retStat = xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 0");
    if(POSITIVE_RES == retStat) 
      csvFile << xtplPrinter->getPosResValue(0)<<';'<<"1"<<std::endl;
    std::cout<< "   "<<"\r"<< xtplPrinter->getPosResValue(0)<<"\r"<<std::flush;
    delay++;  
    }
    
  //Set target table temperature to 90 degree
  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 1 90");

  //wait for temperature stabilization
  delay = 0;
  while(delay != 1200){ /*~2min*/
    // Delay 100ms
    Sleep(100); 
    retStat = xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 0");
    if(POSITIVE_RES == retStat) {
      csvFile << xtplPrinter->getPosResValue(0)<<';'<<"90"<<std::endl;
      std::cout<< xtplPrinter->getPosResValue(0)<<"\r"<<std::flush;
    }
    delay++;  
  }

  /* Stop temperature controller */
  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --temperature 1 0");
  csvFile.close();
  return SUCCESS;
}