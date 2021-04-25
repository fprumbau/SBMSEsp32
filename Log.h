#ifndef LOG_H
#define LOG_H

#define LOGSIZE 10

#include <ArduinoJson.h>

class LOG {
  private:          
        String logEvents[LOGSIZE];          
  public:
        LOG();      
        String get(int index);
        void print(bool web);
        bool append(String logEntry);
};

#endif
