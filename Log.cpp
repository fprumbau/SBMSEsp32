#include "Log.h"

#include "global.h"

#define len(x) sizeof(x)/sizeof(x[0])

String LOG::load() {
  String msg = "Mounting FS...";
  if(!SPIFFS.begin()) {
    msg+="Failed to mount file system (read)\n";
    return msg;
  }
  File logFile = SPIFFS.open("/log.json", "r");
  if(!logFile){
    msg+="Failed to open config file\n";
    return msg;
  }
  size_t size = logFile.size();
  Serial.print("size ");
  Serial.println(size);
  if(size > BUFFSIZE) {
     msg+="Config file is to large";
     return msg;
  }
  //allocate a buffer to store contetns of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutble. if you don't use ArduonJson, you may as well use
  // logFile.readString instead
  logFile.readBytes(buf.get(), size);
  
  if(debug) {
    Serial.println(buf.get());
    msg+=buf.get();
    msg+='\n';
  }
 

  DynamicJsonDocument doc(BUFFSIZE);
  //JsonArray& array = doc.parseArray(buf.get()); //ArduinoJson 5
  auto error = deserializeJson(doc, buf.get());
  JsonArray array = doc.to<JsonArray>();

  if(error) {
    msg+="Failed to parse config file\n";
    return msg;
  }

  eventCount = array.size();
  eventIndex = eventCount;
  for(int i = 0; i < eventCount; i++) {
    const char* sst = array[i];
    
    
    if(debug) {
      Serial.println(sst);
      msg+=sst;
      msg+='\n';
    }
    
    logEvents[i] = String(sst);
  }
  
  return msg;
}

String LOG::save() {
  String msg = "";
  if(!SPIFFS.begin()) {
    msg="Failed to mount file system (save)\n";
    return msg;
  }
  //80 Strings a 100 Zeichen sind 8kB
  DynamicJsonDocument doc(BUFFSIZE);;
  //JsonArray& array = doc.createArray(); ArduinoJson 5
  JsonArray array = doc.to<JsonArray>();

  int maxEvents = len(logEvents);
  for(int i = oldest; i < maxEvents; i++) {
    array.add(logEvents[i]);
  }
  if(oldest > 0) {
      for(int j = 0; j < oldest; j++) {
        array.add(logEvents[j]);
        if(debug) {
           msg+=logEvents[j];
           msg+='\n';
        }
      }
  }

  File logFile = SPIFFS.open("/log.json", "w");
  if (!logFile) {
    msg+="Failed to open config file for writing\n";
    return msg;
  }

  //array.printTo(logFile);
  serializeJson(array, logFile);

  Serial.println(msg);
  return msg;
}

void LOG::print(){
  Serial.print("\nEs traten bisher ");
  Serial.print(eventCount);
  Serial.println(" Logevents auf ");

  Serial.print("Oldest: ");
  Serial.println(oldest);

  Serial.print("Eventindex: ");
  Serial.println(eventIndex);
  
  int actualEventCount = length();

  Serial.print("actualEventCount: ");
  Serial.println(actualEventCount);
  
  for(int i = oldest; i < actualEventCount; i++) {
    Serial.println(logEvents[i]);
  }
  if(oldest > 0) {
      for(int j = 0; j < oldest; j++) {
        Serial.println(logEvents[j]);
      }
  }

  Serial.println("Natural order:");
  for(int k = 0; k < actualEventCount; k++) {
    Serial.println(logEvents[k]);
  }
}

bool LOG::append(String logEntry) {
  if(logEntry == "") {
    return false;
  }
  int maxEvents = len(logEvents);
  if(eventIndex < maxEvents) {
    logEvents[eventIndex] = String(logEntry);
    if(oldest > 0) {
      if(oldest < maxEvents) {
        oldest++; //cursor des nun aeltesten Strings erhoehen
      } else {
        oldest=0; //alle ueberschrieben
      }
    } 
  } else { //mit dem ueberschreiben beginnen
    eventIndex=0;
    logEvents[eventIndex] = String(logEntry);
    oldest=1;
  }
  eventIndex++;
  eventCount++;
}

int LOG::length() {
  int sizeOfLogEvts = len(logEvents);
  int maxEvents = min(sizeOfLogEvts, eventCount);
  return maxEvents;
}

String LOG::get(int index) {

  int logSize = length();
  int realIndex = oldest + index;

  if(realIndex < logSize) {
    return logEvents[realIndex]; 
  } else {
    realIndex = realIndex - logSize;
    return logEvents[realIndex];
  }
  
  return logEvents[index];
}
