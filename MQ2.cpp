#include "Arduino.h"
#include "MQ2.h"

MQ2::MQ2(int pin){
  _pin = pin;
  Ro = -1.0;
}

void MQ2::begin(){
  pinMode(_pin, INPUT);
  Ro = MQCalibration();
  Serial.print("Ro: ");
  Serial.print(Ro);
  Serial.println(" kohm");
}


float MQ2::MQResistanceCalculation(int raw_adc) {
  float flt_adc = (float) raw_adc;
  float rO = RL_VALUE * (4095.0 - flt_adc) / flt_adc;
  return rO;
}

float MQ2::MQCalibration() {
  float Ro = 0.0;
  delay(1000);
  // take multiple samples
  for (int i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {
    Ro = Ro + MQResistanceCalculation(analogRead(_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }

  //calculate the average value
  Ro = Ro / ((float) CALIBARAION_SAMPLE_TIMES);

  //divided by RO_CLEAN_AIR_FACTOR yields the Ro according to the chart in the datasheet 
  Ro = Ro / RO_CLEAN_AIR_FACTOR;

  return Ro; 
    Serial.println();
    Serial.print("Ro: ");
    Serial.println(Ro);
}

float MQ2::MQRead() {
  float rs = 0.0;

  for (int i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(analogRead(_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  return rs / ((float) READ_SAMPLE_TIMES);  // return the average
}

float MQ2::MQGetPercentage(float *pcurve) {
  float rs_ro_ratio = MQRead() / Ro;
  return pow(10.0, ((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0]);
}

float MQ2::readCH(){
    float c = 0.0;
    c = MQGetPercentage(CHCurve);
    Serial.println();
    Serial.print("PPM CH: ");
    Serial.println(c,5);
    return 0.0409 * c * 78.00 * 1000;
}
