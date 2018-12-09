#ifndef INVERTER_H
#define INVERTER_H

class Inverter {
  public:
    bool stopBattery;
    int LOW_VOLTAGE_MILLIS = 2850;
    int SOC_LIMIT = 30; //wird aus Config ueberschrieben
    bool isBatteryOn();
};

#endif
