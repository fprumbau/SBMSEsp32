#ifndef LOG_H
#define LOG_H

#define LOGSIZE 10
#define BUFFSIZE LOGSIZE * 80

//SPIFFS config disk
#include <ArduinoJson.h>
#include <FS.h>

class LOG {
  private:  
        int eventCount=0;
        int eventIndex=0; 
        int oldest=0;
        String logEvents[LOGSIZE];          
  public:
        int length();
        String get(int index);
        String load();
        String save();
        void print();
        bool append(String logEntry);
};

#endif
