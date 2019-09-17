#ifndef SBMS_H
#define SBMS_H

#include <Arduino.h>

class SBMS {

  private:
    unsigned int char_off(char c);    
    unsigned long lastChecked;

  public:
    unsigned int sbmsAnalyzer;
    String data = String((char*)0);
    /**
     * SBMS über serielle Schnittstelle auslesen.
     * Wird 'testFixed' gesetzt (s.o.), dann wird
     * hier immer nur ein fester Werte ausgewertet.
     */
    bool read();
    void print();
    long dcmp(int loc, int size,  const char* sbms, int sbmsLen);
  
};

#endif
