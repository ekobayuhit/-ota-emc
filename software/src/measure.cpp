#include "measure.h"
#include "mqtt.h"
#include <math.h>

float daya_aktif;
float _Irms;
float _Vrms;
float Daya_VI;
double COS_pi;
double correctedPi;
float correctedP;
float Faktor_Daya;

const int sensorPin = A0;
//const int ledgreenPin = 5;//D1
//const int ledredPin = 0;//D3
const int S0=2;//D4
const int S1=3;//RX d9
const int S2=0;//TX d10

const unsigned long sampleTime = 100000UL;                           // melakukan sampling 100ms, it is an exact number of cycles for both 50Hz and 60Hz mains
const unsigned long numSamples = 500UL;                               // choose the number of samples to divide sampleTime exactly, but low enough for the ADC to keep up
const unsigned long sampleInterval = sampleTime/numSamples;  // the sampling interval, must be longer than then ADC conversion time
float adc_zero_current = 397;
float adc_zero_voltage = 428;
float sensitivity_acs712=0.066; //66mV/A for ACS712-30A - Look at datasheet
String displaydatanames = "\""+String("Irms")+"\""+","+"\""+String("Vrms")+"\""+","+"\""+String("Real Power")+"\""+","+"\""+String("Apparent Power")+"\""+","+"\""+String("Power Factor")+"\"";
String displaydatavalues="";

float volt_sensorValue, volt_sensorValue1, volt_sensorValue2;
float curr_sensorValue, curr_sensorValue1, curr_sensorValue2;
float volt_Vmax;
float volt_Vmin;
float curr_Vmax;
float curr_Vmin;
float volt_peak2peak;
float curr_peak2peak;

void measure(){
  volt_Vmax=0;
  volt_Vmin=1024;
  curr_Vmax=0;
  curr_Vmin=1024;

  float currentAcc;
  float voltageAcc;
  float sum_inst_power = 0;
  unsigned int count = 0;
  unsigned long prevMicros = micros() - sampleInterval ;
  while (count < numSamples)
  {
    if (micros() - prevMicros >= sampleInterval)
    {
      //A) Read in raw current and voltage samples
      //Set 8-1 amux to position 5
      digitalWrite(S2, HIGH);
      digitalWrite(S1, LOW);
      digitalWrite(S0, HIGH);
      float adc_voltage = analogRead(sensorPin);// - adc_zero_voltage;

      volt_sensorValue1 = volt_Vmax;
	    volt_sensorValue2 = volt_Vmin;
      curr_sensorValue1 = curr_Vmax;
	    curr_sensorValue2 = curr_Vmin;

      //Set 8-1 amux to position 6
      digitalWrite(S2, HIGH);
      digitalWrite(S1, HIGH);
      digitalWrite(S0, LOW);
      float adc_current = analogRead(sensorPin);// - adc_zero;
      //
	    if(adc_voltage > volt_sensorValue1){
		     volt_Vmax = adc_voltage;
	     }
	    else if (adc_voltage < volt_sensorValue2){
		      volt_Vmin = adc_voltage;
	     }
       else {
         volt_Vmax = volt_sensorValue1;
  	     volt_Vmin = volt_sensorValue2;
       }
       //
       if(adc_current > curr_sensorValue1){
 		      curr_Vmax = adc_current;
 	     }
 	     else if (adc_current < curr_sensorValue2){
 		      curr_Vmin = adc_current;
 	     }
        else {
          curr_Vmax = curr_sensorValue1;
   	      curr_Vmin = curr_sensorValue2;
        }
      //-----------------------------------------------------------------------------
      // B) Apply digital low pass filters to extract the 2.5 V or 1.65 V dc offset,
      //     then subtract this - signal is now centred on 0 counts.
      //-----------------------------------------------------------------------------
      float Ioffset = (adc_zero_current  + (float)(adc_current-adc_zero_current)/1024);
      adc_current-=Ioffset;
      float Voffset = (adc_zero_voltage + (float)(adc_voltage-adc_zero_voltage)/1024);
      adc_voltage-=Voffset;
      //-----------------------------------------------------------------------------
      // C) Root-mean-square method current
      //-----------------------------------------------------------------------------
      //currentAcc += (adc_current * adc_current);
      //voltageAcc += (adc_voltage * adc_voltage);
      //-----------------------------------------------------------------------------
      // D) Instantaneous power calc
      //-----------------------------------------------------------------------------
      float inst_power = adc_current*adc_voltage;
      sum_inst_power +=inst_power;
      count++;
      prevMicros=micros();
    }
  }
  curr_peak2peak = ((curr_Vmax-curr_Vmin)*3300)/1024;
  _Irms = (curr_peak2peak/1.4)/185;
  _Irms = _Irms - 0.35;
  //_Irms = 78.57*curr_peak2peak*curr_peak2peak*curr_peak2peak - 56.15*curr_peak2peak*curr_peak2peak + 14.21*curr_peak2peak - 0.101;
  if( _Irms <0 ){_Irms=0;}
  volt_peak2peak = volt_Vmax-volt_Vmin;
  _Vrms = 0.008*volt_peak2peak*volt_peak2peak - 0.804*volt_peak2peak + 20.6;
  if( _Vrms <0 ){_Vrms=0;}
  float daya_bit = sum_inst_power/(float)numSamples;
  daya_aktif = 5e-9*daya_bit*daya_bit*daya_bit - 5e-5*daya_bit*daya_bit + 0.192*daya_bit + 27.02;
  //if(isnan(daya_aktif)==1){daya_aktif=0;}
  if( daya_aktif <0 ){daya_aktif=0;}
  Daya_VI = _Vrms*_Irms;
  if( Daya_VI <0 ){Daya_VI=0;}
  COS_pi = daya_aktif/Daya_VI;
  if(COS_pi < 0){COS_pi=0;}
  if(COS_pi > 1){COS_pi=1;}
  double pi=acos(COS_pi);
  correctedPi=pi-30;//0.5;//radians(30); //thephase shift of zmpt101b is 30 degree
  if(correctedPi < 0){correctedPi=0;}
  if(correctedPi > 1){correctedPi=1;}
  correctedP = cos(correctedPi)*Daya_VI;
  Faktor_Daya = correctedP/Daya_VI;
  //digitalWrite(ledgreenPin, HIGH);
  /*if(mqtt_connected()){
    Serial.println("Publish data");
    mqtt_publish(mqtt_pub_irms, String(_Irms));
    mqtt_publish(mqtt_pub_vrms, String(_Vrms));
    mqtt_publish(mqtt_pub_realpower, String(correctedP));
    mqtt_publish(mqtt_pub_apparentpower, String(Daya_VI));
    mqtt_publish(mqtt_pub_powerfactor, String(Faktor_Daya));
    //digitalWrite(ledgreenPin, LOW);
  }*/
  displaydatavalues = String("Irms")+String(":")+String(_Irms)+String(" A")+","+String("Vrms")+String(":")+String(_Vrms)+String(" V")+","+String("Real Power")+String(":")+String(correctedP)+String (" W")+","+String("Apparent Power")+String(":")+String(Daya_VI)+String(" VA")+","+String("Power Factor")+String(":")+String(Faktor_Daya);
}
