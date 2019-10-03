#ifndef BATT_H
#define BATT_H


class Battery {

  private:
    
  public:
    int cv[8]; //aktuelle Zellspannungen    
    int cvErrChg[8]; //Zellspannungsfehlercounter Charger
    int cvErrInv[8];  //Zellspannungsfehlercounter Inverter
    void print();
    bool isOn();
    void controlFans();
    void checkCellVoltages();    
};

#endif
