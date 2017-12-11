/*
*  signalAmplifierTest.h
*
*  Created on: 22 nov 2017
*      Author: PZ
*/
#include "pulseSignalHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>

/* Private functions declaration */
static u16 selectedResistanceVal(u8 val);
static char adcOffsetCoeffLoaded = FALSE;
static void performAdcCalibration(cmdHandler* xtplPrinter);
static u16 dacVoltageToRawDac(double voltage);

adc0coeff adc0coeffTab[ADC_OFFSET_COEFF_NUM];
adc1coeff adc1coeffTab[ADC_OFFSET_COEFF_NUM];

/* Public Function Definitions*/
char initAmplifiersBoard(cmdHandler* xtplPrinter) {

#ifdef USE_ADC_SCALING
  if (adcOffsetCoeffLoaded == FALSE) {
    
    std::fstream adc0_coeffFile("adc0coeffs.dat", std::ios::binary | std::ios::in);
    std::fstream adc1_coeffFile("adc1coeffs.dat", std::ios::binary | std::ios::in);
    if ((FALSE == adc0_coeffFile.good()) || (FALSE == adc1_coeffFile.good())) {
      std::cout << "V#: Missing calibration files...."     << std::endl;
      std::cout << "V#: Perform automatic calibration...." << std::endl;
      performAdcCalibration(xtplPrinter);
    }
    //Load adcCoeffs to adc0coeffTab[ADC_OFFSET_COEFF_NUM]
    adc0_coeffFile.read(reinterpret_cast<char *>(&adc0coeffTab), sizeof(adc0coeffTab));
    adc0_coeffFile.close();
    //Load adcCoeffs to adc1coeffTab[ADC_OFFSET_COEFF_NUM]
    adc1_coeffFile.read(reinterpret_cast<char *>(&adc1coeffTab), sizeof(adc1coeffTab));
    adc1_coeffFile.close();

    adcOffsetCoeffLoaded = TRUE;
  }
#endif // !USE_ADC_SCALING
#ifdef USE_ADC_CALIBRATION_AC
#endif //USE_ADC_CALIBRATION_AC
  return 0;
}

char generatePulse(cmdHandler* xtplPrinter, pulseParams inParams, pulseRes *response) {

  std::stringstream cmd;

  cmd << "ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --digital_out_ctrl "
      << (int)(inParams.relaySelection) << " " << (int)inParams.lineresistance;
  xtplPrinter->sendCmd(cmd.str().c_str());
  cmd.str("");

  cmd << "ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_a 1 "
      << inParams.dac0val << " " << inParams.dac1val << " " 
      << (int)(inParams.duration*100000);
  xtplPrinter->sendCmd(cmd.str().c_str());
  cmd.str("");

  /* Wait in loop for valid resoults*/
  do {
    cmd << "ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_a 0";
    xtplPrinter->sendCmd(cmd.str().c_str());
    cmd.str("");
  } while (0 != xtplPrinter->getPosResValue(0));

  /* get data from adc and calculate output values*/
  /* Adding ADC0/ADC1 correction to result */

double adc0_raw = (double)xtplPrinter->getPosResValue(ADC0_RETURN_POSITION);
double adc1_raw = (double)xtplPrinter->getPosResValue(ADC1_RETURN_POSITION);
double adc0 = adc0_raw;
double adc1 = adc1_raw;

#ifdef USE_ADC_SCALING
  //ADC0 calculation
  for (int i = 0; i < sizeof(adc0coeffTab)/sizeof(adc0coeff) - 1; i++) {
    if ((adc0_raw > adc0coeffTab[i].raw) && (adc0_raw < adc0coeffTab[i + 1].raw)) {
      adc0 = adc0coeffTab[i].a * adc0_raw + adc0coeffTab[i].b;
      //When found proper section escape the loop
      break;
    }
  }
  for (int i = 0; i < sizeof(adc1coeffTab)/sizeof(adc1coeff) - 1; i++) {
    if ((adc1_raw> adc1coeffTab[i].raw) && (adc1_raw < adc1coeffTab[i + 1].raw)) {
      adc1 = adc1coeffTab[i].a * adc1_raw + adc1coeffTab[i].b;
      //When found proper section escape the loop
      break;
    }
  }
#endif // USE_ADC_SCALING

  double voltageDropOnLines = abs(AMPLIFIER_KU*(inParams.dac1val - inParams.dac0val)*DAC_V_RESOLUTION);
  u16 selectedLineResistance = selectedResistanceVal(inParams.lineresistance);
  double rawAbsCurrent = 0;
  response->diffVoltage = ((adc1 - adc0)*ADC_V_RESOLUTION)*ADC_FACTOR;
  response->avgCurrent = (response->diffVoltage / selectedLineResistance)*MICRO_AMPER_MUL;
  rawAbsCurrent = abs(response->avgCurrent / MICRO_AMPER_MUL);
  response->calcResistance = (voltageDropOnLines - (2 * rawAbsCurrent * selectedLineResistance)) / rawAbsCurrent;
  return 0;
}

char setSignalForm(cmdHandler* xtplPrinter, signalForm_t signalForm) {
  std::stringstream cmd;
  std::string signalPath;

  switch (signalForm) {
  case sinus:
    signalPath = "..\\config\\sinus.sig";
    break;
  case saw:
    signalPath = "..\\config\\saw.sig";
    break;
  case rectangle:
    signalPath = "..\\config\\rectangle.sig";
    break;
  case user:
    break;
  default:
    break;
  }
  xtplPrinter->sendCmd("ics_client.exe --sys_config_buffer 127.0.0.1 42100 --config 1 2048");

  cmd << "ics_client.exe --sys_client_wr_rd_buffer 127.0.0.1 42100 --wr_s 0 0 \""
    << signalPath << "\"";
  xtplPrinter->sendCmd(cmd.str().c_str());
  cmd.str("");

  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --signal_form_upload 0 2048");

  return 0;
}

char setSignalParameters(cmdHandler* xtplPrinter, signalParameters sigParam) {
  
  std::stringstream cmd;

  cmd << "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --signal_parameters 0 0 0 "
    << (int)sigParam.dcOffsetChannel         << " "
    << (u16)(sigParam.dcOffsetValue * 65535/AMPLIFIER_KU) << " "
    << (u16)(sigParam.amplitude *     65535/AMPLIFIER_KU) << " "
    << (u16)(sigParam.freqMul)               << " "
    << (u16)(sigParam.freqDiv); 
  xtplPrinter->sendCmd(cmd.str().c_str());
  
  cmd.str("");

  return 0;
}

char startSignalGeneration(cmdHandler* xtplPrinter) {
  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --signal_start");
  return 0;
}

char stopSignalGeneration(cmdHandler* xtplPrinter) {
  xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --signal_stop");
  return 0;
}

char relayAndResistanceSelection(cmdHandler* xtplPrinter, u8 relaySelection, u8 selectedResistance) {
  std::stringstream cmd;
  cmd << "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --digital_out_ctrl "
    << (int)relaySelection << " "
    << (int)selectedResistance;
  xtplPrinter->sendCmd(cmd.str().c_str());
  cmd.str("");
  return 0;
}

char getSignalMeasResults(cmdHandler* xtplPrinter, signalParameters sigParam, u16 measDuration, u8 selectedResistance, signalRes *response) {

  std::stringstream cmd;

  cmd << "ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --set_adc_a_meas_duration " << measDuration;
  xtplPrinter->sendCmd(cmd.str().c_str());
  cmd.str("");
  do {
    xtplPrinter->sendCmd("ics_client.exe --dev_xtpl_printer 127.0.0.1 42100 --get_adc_a_result");
  } while (xtplPrinter->getPosResValue(0) != 0);


  // ADC1 is corresponding to DAC1!
  u64 adc0_01_raw; //ADC0>ADC1
  u64 adc1_01_raw; //ADC0>ADC1
  u64 adc0_10_raw; //ADC0<ADC1
  u64 adc1_10_raw; //ADC0<ADC1

  // Get result ADC0 > ADC1
  adc0_01_raw = xtplPrinter->getPosResValue(3);
  adc1_01_raw = xtplPrinter->getPosResValue(4);
  //Get result ADC0 < ADC1
  adc0_10_raw = xtplPrinter->getPosResValue(5);
  adc1_10_raw = xtplPrinter->getPosResValue(6);
  // Convert results to Voltage value 

  u64 adc0_01 = adc0_01_raw; //ADC0>ADC1
  u64 adc1_01 = adc1_01_raw; //ADC0>ADC1
  u64 adc0_10 = adc0_10_raw; //ADC0<ADC1
  u64 adc1_10 = adc1_10_raw; //ADC0<ADC1

//#ifdef USE_ADC_SCALING
//
//  //ADC0 calculation
//  for (int i = 0; i < sizeof(adc0coeffTab) / sizeof(adc0coeff) - 1; i++) {
//    if ((adc0_01_raw > adc0coeffTab[i].raw) && (adc0_01_raw < adc0coeffTab[i + 1].raw)) {
//      adc0_01 = adc0coeffTab[i].a * adc0_01_raw + adc0coeffTab[i].b;
//      //When found proper section escape the loop
//      break;
//    }
//  }
//  for (int i = 0; i < sizeof(adc0coeffTab) / sizeof(adc0coeff) - 1; i++) {
//    if ((adc0_10_raw > adc0coeffTab[i].raw) && (adc0_10_raw < adc0coeffTab[i + 1].raw)) {
//      adc0_10 = adc0coeffTab[i].a * adc0_10_raw + adc0coeffTab[i].b;
//      //When found proper section escape the loop
//      break;
//    }
//  }
//  for (int i = 0; i < sizeof(adc1coeffTab) / sizeof(adc1coeff) - 1; i++) {
//    if ((adc1_01_raw > adc1coeffTab[i].raw) && (adc1_01_raw < adc1coeffTab[i + 1].raw)) {
//      adc1_01 = adc1coeffTab[i].a * adc1_01_raw + adc1coeffTab[i].b;
//      //When found proper section escape the loop
//      break;
//    }
//  }
//  for (int i = 0; i < sizeof(adc1coeffTab) / sizeof(adc1coeff) - 1; i++) {
//    if ((adc1_10_raw> adc1coeffTab[i].raw) && (adc1_10_raw < adc1coeffTab[i + 1].raw)) {
//      adc1_10 = adc1coeffTab[i].a * adc1_10_raw + adc1coeffTab[i].b;
//      //When found proper section escape the loop
//      break;
//    }
//  }
//#endif // USE_ADC_SCALING


  double adc0_01_V = adc0_01*ADC_V_RESOLUTION*ADC_FACTOR;
  double adc1_01_V = adc1_01*ADC_V_RESOLUTION*ADC_FACTOR;
  double adc0_10_V = adc0_10*ADC_V_RESOLUTION*ADC_FACTOR;
  double adc1_10_V = adc1_10*ADC_V_RESOLUTION*ADC_FACTOR;
  response->adc0_01_V = adc0_01_V;
  response->adc1_01_V = adc1_01_V;
  response->adc0_10_V = adc0_10_V;
  response->adc1_10_V = adc1_10_V;


#define DEBUGCOUT
#ifdef DEBUGCOUT
  std::cout << "ADC0 > ADC1" << std::endl;
  std::cout << "adc0_01_V:" << adc0_01_V << std::endl;
  std::cout << "adc1_01_V:" << adc1_01_V << std::endl;
  std::cout << "ADC0 < ADC1" << std::endl;
  std::cout << "adc0_10_V:" << adc0_10_V << std::endl;
  std::cout << "adc1_10_V:" << adc1_10_V << std::endl;
#endif

  double DAC1_AVG_V = adc1_10_V;
  double DAC0_AVG_V;
  if (sigParam.dcOffsetChannel == DAC1) {
    DAC0_AVG_V = (sigParam.amplitude + sigParam.dcOffsetValue) - DAC1_AVG_V;
  }
  else {
    DAC0_AVG_V = sigParam.amplitude - (DAC1_AVG_V - sigParam.dcOffsetValue);
  }
  
  double DROP_V = adc1_10_V - adc0_10_V;

#ifdef DEBUGCOUT
  std::cout << "DAC results:" << std::endl;
  std::cout << "DAC0_AVG_V: " << DAC0_AVG_V << std::endl;
  std::cout << "DAC1_AVG_V: " << DAC1_AVG_V << std::endl;
  std::cout << "DROP V" << std::endl;
#endif

  /* Calculate averrage current */
  double I_AVG = (DROP_V) / selectedResistanceVal(selectedResistance);
#ifdef DEBUGCOUT
  std::cout << "Calculated Average Current:" << std::endl;
  std::cout << "I_AVG: " << I_AVG << std::endl;
#endif

  double calculateR = (DAC1_AVG_V - 2 * DROP_V - DAC0_AVG_V) / I_AVG;
#ifdef DEBUGCOUT
  std::cout << "Calculated Resistance: " << calculateR << std::endl;
#endif // DEBUGCOUT
  response->calcResistance = calculateR;
  response->avgCurrent = I_AVG;
  response->avgDiffVoltage = DROP_V;
  return 0;
}


/* Private Function Definitions*/
static u16 selectedResistanceVal(u8 val) {
  switch (val) {
  case LINE_RES_55K:
    return 55000;
    break;
  case LINE_RES_5K:
    return 5000;
    break;
  case LINE_RES_50K:
    return 50000;
    break;
  default:
    /* TODO create handling error here*/
    return 1;
    break;
  }
}


static void performAdcCalibration(cmdHandler* xtplPrinter) {
  //variables to store intermediate results
  calibrationData calibrationTab[ADC_OFFSET_COEFF_NUM];
  adc0coeff       adc0calibData[ADC_OFFSET_COEFF_NUM];
  adc0coeff       adc1calibData[ADC_OFFSET_COEFF_NUM];
  std::stringstream cmd;

  /* User interaction required*/
  std::cout << std::endl << std::endl;
  std::cout << "V:#:*********************************************************" << std::endl;
  std::cout << "V#: DICONNECT  line resistance (gap) -> tab ENTER to continue";
  getchar();
  std::cout << "V#: ADC calibration START" << std::endl;

  int dotCnt = 0;
  int intIndex = 0;
  std::cout << "V#: CALIBRATION in progress";
  for (double index = 0.1; index < (ADC_OFFSET_COEFF_NUM * 0.1); index=index+0.1) {
    dotCnt++;
    std::cout << ".";
    cmd << "ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_a 1 0"
      << " " << dacVoltageToRawDac(index) << " "
      << (int)(200 * 100000);
    xtplPrinter->sendCmd(cmd.str().c_str());
    cmd.str("");
    do {
      cmd << "ics_client.exe --dev_xtpl_printer  127.0.0.1 42100 --pulse_a 0";
      xtplPrinter->sendCmd(cmd.str().c_str());
      cmd.str("");
    } while (0 != xtplPrinter->getPosResValue(0));
    calibrationTab[intIndex].rawExpectedAdcRes = (u32)(index / (ADC_V_RESOLUTION * ADC_FACTOR));
    calibrationTab[intIndex].rawAdc0Res = (u32)xtplPrinter->getPosResValue(1);
    calibrationTab[intIndex].rawAdc1Res = (u32)xtplPrinter->getPosResValue(2);
    intIndex++;
    if (dotCnt == 20) {
      std::cout << '\r';
      std::cout << "                                                     ";
      std::cout << '\r';
      std::cout << "V#: CALIBRATION in progress";
      dotCnt = 0;
    }
  }
  std::cout << '\r';
  std::cout << "                                                     \r";
  //Algorithm to calculate a, b coeffs of ADC0 + store it in file
  for(int i = 0; i < ADC_OFFSET_COEFF_NUM-1; i++) {
    adc0calibData[i].raw = calibrationTab[i].rawAdc0Res;
    adc0calibData[i].a = 1.0*(calibrationTab[i + 1].rawExpectedAdcRes - calibrationTab[i].rawExpectedAdcRes) / (calibrationTab[i + 1].rawAdc0Res - calibrationTab[i].rawAdc0Res);
    adc0calibData[i].b = 1.0*calibrationTab[i].rawExpectedAdcRes -(1.0*adc0calibData[i].a * calibrationTab[i].rawAdc0Res);
  }

  //Algorithm to calculate a, b coeffs of ADC1 + store it in file
  for (int i = 0; i < ADC_OFFSET_COEFF_NUM-1; i++) {
    adc1calibData[i].raw = calibrationTab[i].rawAdc1Res;
    adc1calibData[i].a = 1.0*(calibrationTab[i + 1].rawExpectedAdcRes - calibrationTab[i].rawExpectedAdcRes) /
                         (calibrationTab[i + 1].rawAdc1Res - calibrationTab[i].rawAdc1Res);
    adc1calibData[i].b = 1.0*calibrationTab[i].rawExpectedAdcRes - (1.0*adc1calibData[i].a * calibrationTab[i].rawAdc1Res);
  }
  std::cout << "V:# ADC Calibration END" << std::endl;
  std::cout << "V:#:*********************************************************" << std::endl;
  std::fstream adc0_dat("adc0coeffs.dat", std::ios::out | std::ios::binary);
  std::fstream adc1_dat("adc1coeffs.dat", std::ios::out | std::ios::binary);
  adc0_dat.write(reinterpret_cast<char *>(&adc0calibData), sizeof(adc0calibData));
  adc0_dat.close();
  adc1_dat.write(reinterpret_cast<char *>(&adc1calibData), sizeof(adc1calibData));
  adc1_dat.close();
  std::cout << "Press Enter to continue" << std::endl;
}

static u16 dacVoltageToRawDac(double voltage) {
  double tempRes = voltage / (AMPLIFIER_KU*DAC_V_RESOLUTION);
  return (u16)tempRes;
}