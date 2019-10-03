#ifndef INVERTER_H
#define INVERTER_H

class Inverter {
  private: 
    bool nacht = false;
    unsigned long lastCheckedMillis = -1; 
    int failureCount = 0;
    const int errLimit = 10;    
  public:
    bool batteryEnabled = false; //wegen Belgienurlaub, Batterie SOLL aus bleiben
    void setBlue();
    void setGreen();
    void setRed();   
    void check(); //Pruefe Werte fuer SOC und cell voltages
    bool stopBattery; //Stopflag, verhindert ein manuelles Anschalten der Batterie
    void starteNetzvorrang(String reason); //Netzbetrieb aktivieren
    bool starteBatterie(String reason); //Batteriebetrieb aktivieren   
    void handleButtonPressed(); //Tastersteuerung, um Inverterstatus zu toggeln 
    void enableBattery(bool flag);
};

#endif 
