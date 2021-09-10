#ifndef BATT_H
#define BATT_H

#include <ArduinoJson.h>

class Battery {

  private:
  
    int LOW_MINIMAL_CV_MILLIS = 2800; //untere Zellspannung in mv
    int LOW_NIGHTLY_CV_MILLIS = 3050; //untere Zellspannung in mv im Nachtmodus
    int CV_HYST = 50; //Anschalten der Batterie erst moeglich, wenn CV jeder Zelle > LOW_MINIMAL_CV_MILLIS + CV_HYST ist    
    int SOC_HYST = 5; //5% Hysterese  
    int cvErrChg[8]; //Zellspannungsfehlercounter Charger
    int cvErrInv[8];  //Zellspannungsfehlercounter Inverter

  public:

    String status;
    bool enabled = false; //wegen Belgienurlaub, Batterie SOLL aus bleiben
    int cv[8]; //aktuelle Zellspannungen    
    int soc = -1; //aktueller Wert State Of Charge
    int socLimit = 70; //wird aus Config ueberschrieben (in %)
    void print();
    bool isOn();
    bool checkCellVoltages();    
    bool isReady2Activate(); //Alle Regeln OK zur Nutzung (SoC + Aktivflag)
   
    
};

#endif
