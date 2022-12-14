#include "Arduino.h"
#include "DSM501A.h"

DSM501A::DSM501A(int pin25, int pin10){
  _pin25 = pin25;
  _pin10 = pin10;
}

void DSM501A::begin(){
  pinMode(_pin25, INPUT);
  pinMode(_pin10, INPUT);
  starttime1 = millis();
  starttime2 = millis();
  starttime3 = millis();
}

float DSM501A::readpm25(){
  duration1 = pulseIn(_pin25, LOW);
  lowpulseoccupancy1 += duration1;
  endtime1 = millis();
  
  if (millis() - starttime1 > sampletime_ms){
    ratio1 = lowpulseoccupancy1/(sampletime_ms*10.0);
    concentration1 = (0.002751*pow(ratio1,2)+0.082511*ratio1-0.009286)*1000;
    lowpulseoccupancy1= 0;
    starttime1=millis();
    return concentration1 < 0.0? 0 : concentration1; 
  }
}

float DSM501A::readpm10(){
  duration2 = pulseIn(_pin10, LOW);
  lowpulseoccupancy2 += duration2;
  endtime2 = millis();
  
  if (millis() - starttime2 > sampletime_ms){
    ratio2 = lowpulseoccupancy2/(sampletime_ms*10.0);
    concentration2 = (0.002751*pow(ratio2,2)+0.082511*ratio2-0.009286)*1000;
    lowpulseoccupancy2= 0;
    starttime2=millis();
    return concentration2 < 0.0? 0 : concentration2; 
  }
}
