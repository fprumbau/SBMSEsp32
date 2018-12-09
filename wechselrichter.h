#ifndef INVERTER_H
#define INVERTER_H

class Inverter {
  public:
    bool stopBattery; //Stopflag, verhindert ein manuelles Anschalten der Batterie
    int LOW_VOLTAGE_MILLIS = 2850;
    int SOC_LIMIT = 30; //wird aus Config ueberschrieben
    bool isBatteryOn();
    void starteNetzvorrang(String reason); //Netzbetrieb aktivieren
    void starteBatterie(String reason); //Batteriebetrieb aktivieren
};

#endif
