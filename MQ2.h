#ifndef MQ2_h
#define MQ2_h

#include "Arduino.h"
// define the load resistance on the board, in kilo ohms
# define RL_VALUE 47.0
// given constant
# define RO_CLEAN_AIR_FACTOR 9.83

// reads 10 times the sensor every 50ms and takes the average
// NOTE: it is encouraged to take more samples during the calibration
# define CALIBARAION_SAMPLE_TIMES 10
# define CALIBRATION_SAMPLE_INTERVAL 50

// reads 5 times the sensor every 50ms and takes the average
# define READ_SAMPLE_TIMES 5
# define READ_SAMPLE_INTERVAL 50

class MQ2{
  public:
    MQ2(int pin);
    void begin();
    float readCH();

  private:
    int _pin;
    float CHCurve[3] = {2.3, 0.21, -0.47};                                                        
    float Ro = -1.0;

    float values;  // array with the measured values in the order: lpg, CO and smoke
    
    float MQRead();
    float MQGetPercentage(float *pcurve);
    float MQCalibration();
    float MQResistanceCalculation(int raw_adc);
};

# endif
