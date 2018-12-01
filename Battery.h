#ifndef BATTERY_H
#define BATTERY_H

#include "Vars.h"

class Battery {
  private:
    Vars vars;
  public:
    Battery(Vars &vars);
    bool stopBattery;
    int LOW_VOLTAGE_MILLIS = 2850;
    int SOC_LIMIT = 30; //wird aus Config ueberschrieben
    bool isBatteryOn();
};

#endif
