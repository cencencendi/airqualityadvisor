#ifndef MQ131_h
#define MQ131_h

#include "Arduino.h"
// define the load resistance on the board, in kilo ohms
# define RL_VALUE 47.0
// given constant
# define RO_CLEAN_AIR_FACTOR 1.0

// reads 10 times the sensor every 50ms and takes the average
// NOTE: it is encouraged to take more samples during the calibration
# define CALIBARAION_SAMPLE_TIMES 10
# define CALIBRATION_SAMPLE_INTERVAL 50

// reads 5 times the sensor every 50ms and takes the average
# define READ_SAMPLE_TIMES 5
# define READ_SAMPLE_INTERVAL 50

class MQ131{
  public:
    MQ131(int pin);
    void begin();
    float readO3();

  private:
    int _pin;                                                     
    float Ro = -1.0;

    float values;  // array with the measured values in the order: lpg, CO and smoke
    
    float MQRead();
    float MQCalibration();
    float MQResistanceCalculation(int raw_adc);

;};

# endif
