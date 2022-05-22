#include "Log.h"

#include "global.h"

LOG::LOG() {
  for(int i=0;i<LOGSIZE;i++) {
    logEvents[i] = "";
  }
}

void LOG::print(bool web){

  if(!false) {      
      for(int j = 0; j < LOGSIZE; j++) {
          Serial.println(logEvents[j]);
      }  
  } else {
      for(int j = 0; j < LOGSIZE; j++) {
          wc.sendClients(logEvents[j].c_str());
      }
  }
  
}

bool LOG::append(String logEntry) {
  logEntry.trim();
  if(logEntry.length() > 0) {
  
    for(int i = LOGSIZE-2; i>=0; i--) {
      logEvents[i+1] = logEvents[i];
    }
    logEvents[0] = timeClient.getFormattedTime() + " " + logEntry;
  }
}

String LOG::get(int index) {  
  return logEvents[index];
}
