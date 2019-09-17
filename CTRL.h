#ifndef CTRL_H
#define CTRL_H

class CTRL {
    private:
      long uptime;
      
    public:
      CTRL(long millis);
      bool isUpForSeconds(int seconds);
      bool fansRunning();
      void fansOn();
      void fansOff();
      void print(); 
};      
#endif
