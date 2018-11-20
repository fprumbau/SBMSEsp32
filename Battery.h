#ifndef BATTERY_H
#define BATTERY_H

class Battery {
  public:
    bool stopBattery;
    int LOW_VOLTAGE_MILLIS = 2850;
    int SOC_LIMIT = 30; //wird aus Config ueberschrieben
};

#endif
