#ifndef MEASURE_H
#define MEASURE_H
#include <Arduino.h>

extern void measure();
extern float daya_aktif;
extern float _Irms;
extern float _Vrms;
extern float Daya_VI;
extern double COS_pi;
extern double correctedPi;
extern float correctedP;
extern float Faktor_Daya;
extern String displaydatavalues;
extern String displaydatanames;
extern const int sensorPin;
extern const int ledredPin;
extern const int ledgreenPin;
extern const int S0;
extern const int S1;
extern const int S2;
#endif
