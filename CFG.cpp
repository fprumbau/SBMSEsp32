#include "config.h"
#include "global.h"

#define WEBUSER "webUser"
#define WEBPASS "webPass"
#define VEHICLEID "vehicleId"
#define AUTHORIZATION "authorization"
#define TESLAACTIVE "teslaCtrlActive"
#define SOCLIMIT "socLimit"
#define BATTERYENABLED "batteryEnabled"

void CFG::load() {

  if(!LITTLEFS.begin()) {
    return;
  }
  File configFile = LITTLEFS.open("/config.json", "r");
  if(!configFile){
    return;
  }
  size_t size = configFile.size();
  if(size>8192) {
     Log.error(F("Config file is to large"));
     return;
  }
  //allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  if(debugConfig) {
    Log.warning(buf.get());
  }

  DynamicJsonDocument doc(1024);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Log.error(F("Failed to parse config file"));
  }

  if(doc.containsKey(AUTHORIZATION) && doc.containsKey(VEHICLEID)) {
    
      const char* auth = doc[AUTHORIZATION];
      const char* vehicleId = doc[VEHICLEID];

      if(debugConfig) {
        serializeJson(doc, Serial);
        Log.error(F("\nInitialisiere Teslakonfig: authorization:|%s|; vehicleId:|%s|" CR), auth, vehicleId);
      }
    
      perry.vehicleId(vehicleId);
      perry.authorization(auth);
  }

  if(doc.containsKey(WEBUSER) && doc.containsKey(WEBPASS)) {
    //Webzugang
    const char* webUser = doc[WEBUSER];
    _webUser = new char[strlen(webUser)+1];
    strcpy(_webUser, webUser); 
  
    const char* webPass = doc[WEBPASS];
    _webPass = new char[strlen(webPass)+1];
    strcpy(_webPass, webPass);

    if(debugConfig) {
      Log.notice(F("\nInitialisiere User/Pw: user:|%s|; password:|%s|" CR), _webUser, _webPass);
    }
  
    //Lese andere Konfigwerte fuer global.h    
    if(doc.containsKey(TESLAACTIVE)) {
      teslaCtrlActive = doc[TESLAACTIVE];
      if(debugConfig) {
        Log.notice(F("%s%T"),TESLAACTIVE, teslaCtrlActive);
      }
    }
    if(doc.containsKey(SOCLIMIT)) {
      battery.socLimit = doc[SOCLIMIT];
      if(debugConfig) {
        Log.notice(F("%s%s"),SOCLIMIT, battery.socLimit);
      }
    }  
    if(doc.containsKey(BATTERYENABLED)) {
      battery.enabled = doc[BATTERYENABLED];  
      if(debugConfig) {
        Log.notice(F("%s%T"),BATTERYENABLED, battery.enabled);;  
      }      
    }
  }
}

bool CFG::save() {
  
  if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    return false;
  }

  DynamicJsonDocument doc(1024);
  
  doc[VEHICLEID] = perry.vehicleId();
  doc[AUTHORIZATION] = perry.authorization();
  doc[TESLAACTIVE] = teslaCtrlActive;
  doc[WEBUSER] = _webUser;
  doc[WEBPASS] = _webPass;
  doc[SOCLIMIT] = battery.socLimit;
  doc[BATTERYENABLED] = battery.enabled;

  File configFile = LITTLEFS.open("/config.json", "w");
  if (!configFile) {
    Log.error(F("Failed to open config file for writing"));
    return false;
  }
  if(debugConfig) {
    serializeJson(doc, Serial);
  }
  serializeJson(doc, configFile);
  Log.notice(F("\nKonfiguration wurde erfolgreich gespeichert."));

  configFile.flush();
  configFile.close();
  
  return true;
}

void CFG::set(const String& keyVal) {

  String key = getValue(keyVal, ':', 0);
  String val = getValue(keyVal, ':', 1);

  set(key.c_str(), val.c_str());
}

void CFG::set(const char* key, const char* val) {

  if(debugConfig) {
    Log.warningln(F("Set config value '%s' to '%s'; Still has to be saved"), key, val);
  }

  String keyStr = String(key);

  if(keyStr == VEHICLEID) {
      perry.vehicleId(val);
  } else if(keyStr == AUTHORIZATION) {
      perry.authorization(val);
  } else if(keyStr == WEBUSER) {
      _webUser = new char[strlen(val)+1];
      strcpy(_webUser, val);    
  } else if(keyStr == WEBPASS) {
      _webPass = new char[strlen(val)+1];
      strcpy(_webPass, val);
  } else if(keyStr == TESLAACTIVE) {
    if(strcmp(val,"1") || strcmp(val,"true")) {
      teslaCtrlActive=true;
    } else {
      teslaCtrlActive=false;
    }
  } else if(keyStr == SOCLIMIT) {
      battery.socLimit = String(val).toInt();
  } else {
      Log.warning(F("Fuer diesen Konfigwert wurde keine Verarbeitung gefunden: %s" CR), key);
  }
}

String CFG::getValue(String data, char separator, int index) {
  
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

char* CFG::webUser() {
  return _webUser;
}
char* CFG::webPass() {
  return _webPass;
}
const char* CFG::load(const String& key) {
  if(!LITTLEFS.begin()) {
    return NULL;
  }
  File configFile = LITTLEFS.open("/config.json", "r");
  if(!configFile){
    return NULL;
  }
  size_t size = configFile.size();
  if(size>8192) {
     Log.error(F("Config file is to large"));
     return NULL;
  }
  //allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  if(debugConfig) {
    Log.warning(buf.get());
  }

  DynamicJsonDocument doc(1024);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Log.error(F("Failed to parse config file"));
  }
  
  const char* val = doc[key];    
  return val;        
}       
bool CFG::save(const String& key, const String& val) {

  if(!LITTLEFS.begin()) {
    return NULL;
  }
  File configFile = LITTLEFS.open("/config.json", "r");
  if(!configFile){
    return NULL;
  }
  size_t size = configFile.size();
  if(size>8192) {
     Log.error(F("Config file is to large"));
     return NULL;
  }
  //allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  if(debugConfig) {
    Log.warning(buf.get());
  }

  DynamicJsonDocument doc(1024);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Log.error(F("Failed to parse config file"));
  }  

  doc[key]=val;  

  
  Log.warning(F("Key: %s; Value: %s" CR), key, val);
  
  serializeJson(doc, Serial);
  serializeJson(doc, configFile);
  Log.notice(F("\nKonfiguration wurde erfolgreich gespeichert."));

  configFile.flush();
  configFile.close();  
}

void CFG::print() {
  Log.warning(F("--------------------------------"));
  Log.warning(F("_webUser: %s" CR), _webUser);
  Log.warning(F("_webPass: %s" CR), _webPass);
  Log.warning(F("debug: %s" CR), debug);
  Log.warning(F("debugSma: %s" CR), debugSma);
  Log.warning(F("debugSbms: %s" CR), debugSbms);
  Log.warning(F("debugJson: %s" CR), debugJson);
  Log.warning(F("debugConfig: %s" CR), debugConfig);
  Log.warning(F("debugRelais: %s" CR), debugRelais);
  Log.warning(F("debugCharger: %s" CR), debugCharger);
  Log.warning(F("debugBattery: %s" CR), debugBattery);
  Log.warning(F("debugInverter: %s" CR), debugInverter); 
}

long CFG::secondsSince(long milliSeconds) {
  return (millis() - milliSeconds) / 1000;
}
