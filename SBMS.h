#ifndef SBMS_H
#define SBMS_H

class SBMS {

  private:
    unsigned int char_off(char c);    

  public:

    /**
     * SBMS über serielle Schnittstelle auslesen.
     * Wird 'testFixed' gesetzt (s.o.), dann wird
     * hier immer nur ein fester Werte ausgewertet.
     */
    bool read();
    long dcmp(int loc, int size,  const char* sbms, int sbmsLen);
  
};

#endif
