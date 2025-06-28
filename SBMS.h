#ifndef SBMS_H
#define SBMS_H

#include <Arduino.h>

#define SBMS_DATA_BUFFER_SIZE 70 // Entspricht STRING_BUFFER_SIZE

class SBMS {

  private:
    unsigned int char_off(char c);    
    unsigned long lastChecked;
    unsigned long lastDisplayUpdate;
    unsigned int oldSoc;
    char dataBuffer[SBMS_DATA_BUFFER_SIZE]; // Statischer Puffer
    void readSoc(const char* txt, int len);

  public:
    SBMS() {
      memset(dataBuffer, 0, SBMS_DATA_BUFFER_SIZE); // Initialisiere Puffer
    }
    /**
    * Getter für dataBuffer
    * @return const char* auf den internen Puffer
    */
    const char* getData() const {
      return dataBuffer;
    }  
    /**
    * Gibt die Länge des aktuellen dataBuffer
    * @return Länge des Strings in dataBuffer
    */
    size_t getDataLength() const {
      return strlen(dataBuffer);
    }
    /**
    * Prüft, ob ein bestimmtes Zeichen an Position pos im dataBuffer steht
    * @param pos Position im Puffer
    * @return Zeichen an Position pos oder '\0' wenn außerhalb
    */
    char getDataCharAt(size_t pos) const {
      if (pos < SBMS_DATA_BUFFER_SIZE) {
        return dataBuffer[pos];
      }
      return '\0';
    }
  
    /**
     * SBMS über serielle Schnittstelle auslesen.
     * Wird 'testFixed' gesetzt (s.o.), dann wird
     * hier immer nur ein fester Werte ausgewertet.
     */
    bool read();
    void print();
    long dcmp(int loc, int size,  const char* sbms, int sbmsLen);
  
};

extern SBMS sbms;//SBMS solar battery charger functions, uncompress etc.

#endif
