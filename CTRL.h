#ifndef CTRL_H
#define CTRL_H

class CTRL {
    private:
      
    public:
      int string1 = 0;
      int string2 = 0;
      float vorlauf = 0;
      float ruecklauf = 0;
      int pegel = 0;
      int temp = 0;
      bool isUpForSeconds(int seconds);
      void print(); 
      void retrieveData();
      bool debugCtrl=false;
      unsigned long time_now = 0;
};      
#endif
