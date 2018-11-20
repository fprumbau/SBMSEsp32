#ifndef CFG_H
#define CFG_H

//SPIFFS config disk
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include "Battery.h"

class CFG {
  private:
        bool debug = false;
        int _socLimit = 30;
  public:
        bool load();
        bool save();
        int getSocLimit();     
        void setSocLimit(int socLimit);
        void init(Battery& battery);
};

#endif
