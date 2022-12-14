#include "Arduino.h"

#ifndef DSM501A_H
#define DSM501A_H

#define DSM501A_WARMUP_TIME 60000


class DSM501A{
  public:
    DSM501A(int pin25, int pin10);
    float readpm25();
    float readpm10();
    unsigned long starttime1,starttime2,starttime3;
    unsigned long sampletime_ms = 30000;
    void begin();

  private:
    int _pin25;
    int _pin10;
    unsigned long duration1,duration2,endtime1,endtime2;
    unsigned long lowpulseoccupancy1,lowpulseoccupancy2;
    float ratio1,ratio2;
    float concentration1,concentration2;
};

#endif
