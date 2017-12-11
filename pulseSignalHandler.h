#pragma once
/*
*  pulseSignalHandler.h
*
*  Created on: 23 nov 2017
*      Author: PZ
*/
#include "cmdHandler.h"
#include "types.h"
/* Defines */
#define USE_ADC_SCALING
#define DEBUGCOUT

#define ADC0_RETURN_POSITION 1
#define ADC1_RETURN_POSITION 2
#define ADC_OFFSET_COEFF_NUM 698 //<0.1V - 69.9V>, step 0.1V -> 698 = (69.9-0.1)/0.1

#define CONVERSION_END   0
#define ADC_V_RESOLUTION 0.000000149
/* BOARD SPECYFIC */
/* Look into documentation  for XYZ values*/
/* ADC_FACTOR = (Y + Z)/(Y + Z + X))*/
#define ADC_FACTOR       40
#define DAC_V_RESOLUTION 0.000015259

#define LINE_RES_5K      0x01
#define LINE_RES_50K     0x02
#define LINE_RES_55K     0x00
#define LINE_RES_0K      0x03

#define RELAY_1          0xFE
#define RELAY_2          0xFD
#define RELAY_3          0xFB
#define RELAY_4          0xF7
#define RELAY_5          0xEF
#define RELAY_6          0xDF
#define RELAY_7          0xBF
#define RELAY_8          0x7F

#define AMPLIFIER_KU     100

#define MICRO_AMPER_MUL  1000000
#define MILI_AMPER_MUL   1000

#define DAC0  0
#define DAC1  1

typedef enum signalForm_t {
  sinus = 0,
  rectangle,
  saw,
  user
}signalForm_t;

/* pulse types */
struct pulseParams {
  u16 dac0val;        //[raw hexValue]
  u16 dac1val;        //[raw hexValue]
  u16 duration;       //[in miliseconds];
  u8 lineresistance;  //[0x00 - 55k
                      // 0x01 - 5k
                      // 0x02 - 50k
                      // 0x03 -  0k]
  u8  relaySelection; //[0xFE - relay 1, 0x7F - relay 8)
  
};

struct signalRes {
  double avgCurrent;     //[in microAmper]
  double avgDiffVoltage; //[in voltage]
  double calcResistance; //[Ohm]
  double adc0_01_V;
  double adc1_01_V;
  double adc0_10_V;
  double adc1_10_V;
};

struct pulseRes {
  double avgCurrent;     //[in microAmper]
  double diffVoltage;    //[in voltage]
  double calcResistance; //[Ohm]
};

struct adc0coeff {
  u32 raw;
  double a;
  double b;
};

struct adc1coeff {
  u32 raw;
  double a;
  double b;
};

struct calibrationData {
  u32 rawExpectedAdcRes;
  u32 rawAdc0Res;
  u32 rawAdc1Res;
};

struct signalParameters {
  u32 minValLimiter;      //[V]
  u32 maxValLimiter;      //[V]
  u8  dcOffsetChannel;    //[0-DAC0, 1-DAC1]
  double dcOffsetValue;   //[V]
  double amplitude;       //[V]
  u16 freqMul;
  u32 freqDiv;
};



/* Interfaces */
char generatePulse(cmdHandler* xtplPrinter, pulseParams inParams, pulseRes* response);
char initAmplifiersBoard(cmdHandler* xtplPrinter);

char setSignalForm(cmdHandler* xtplPrinter, signalForm_t signalForm);
char setSignalParameters(cmdHandler* xtplPrinter, signalParameters sigParam);
char startSignalGeneration(cmdHandler* xtplPrinter);
char stopSignalGeneration(cmdHandler* xtplPrinter);
char relayAndResistanceSelection(cmdHandler* xtplPrinter, u8 relaySelection, u8 selectedResistance);
char getSignalMeasResults(cmdHandler* xtplPrinter, signalParameters sigParam, u16 measDuration, u8 selectedResistance, signalRes *response);