#ifndef INVERTER_H
#define INVERTER_H

class Inverter {
  private: 
    bool nacht = false;
    unsigned long lastCheckedMillis = -1; 
  public:
    void setBlue();
    void setGreen();
    void setRed();   
    void check(); //Pruefe Werte fuer SOC und cell voltages
    bool stopBattery; //Stopflag, verhindert ein manuelles Anschalten der Batterie
    void starteNetzvorrang(String reason); //Netzbetrieb aktivieren
    void starteBatterie(String reason); //Batteriebetrieb aktivieren   
    void handleButtonPressed(); //Tastersteuerung, um Inverterstatus zu toggeln 
};

#endif 
