#include "config.h"
#include "global.h"

#define WEBUSER "webUser"
#define WEBPASS "webPass"
#define VEHICLEID "vehicleId"
#define AUTHORIZATION "authorization"
#define TESLAACTIVE "teslaCtrlActive"
#define SOCLIMIT "socLimit"
#define BATTERYENABLED "batteryEnabled"
#define BATTERYDAUERBRIEB "batteryDauerbetrieb"
#define BATTERYSTART "batterystart"
#define BATTERYSTOP "batterystop"

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
     Serial.println(F("Config file is to large"));
     return;
  }
  //allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  if(debugConfig) {
    Serial.println(buf.get());
  }

  DynamicJsonDocument doc(1024);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Serial.println(F("Failed to parse config file"));
  }

  if(doc.containsKey(AUTHORIZATION) && doc.containsKey(VEHICLEID)) {
    
      const char* auth = doc[AUTHORIZATION];
      const char* vehicleId = doc[VEHICLEID];

      if(debugConfig) {
        serializeJson(doc, Serial);
        Serial.print(F("\nInitialisiere Teslakonfig: authorization:|"));
        Serial.print(auth);
        Serial.print(F("|; vehicleId:|"));
        Serial.print(vehicleId);
        Serial.println(F("|"));
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
      Serial.print(F("\nInitialisiere User/Pw: user:|"));
      Serial.print(_webUser);
      Serial.print(F("|; password:|"));
      Serial.print(_webPass);
      Serial.println(F("|"));
    }
  
    //Lese andere Konfigwerte fuer global.h    
    if(doc.containsKey(TESLAACTIVE)) {
      teslaCtrlActive = doc[TESLAACTIVE];
      if(debugConfig) {
        Serial.print(TESLAACTIVE);
        Serial.println(teslaCtrlActive);  
      }
    }
    if(doc.containsKey(SOCLIMIT)) {
      battery.socLimit = doc[SOCLIMIT];
      if(debugConfig) {
        Serial.print(SOCLIMIT);
        Serial.println(battery.socLimit);  
      }
    }  
    if(doc.containsKey(BATTERYENABLED)) {
      battery.enabled = doc[BATTERYENABLED];  
      if(debugConfig) {
        Serial.print(BATTERYENABLED);
        Serial.println(battery.enabled);  
      }      
    }
    if(doc.containsKey(BATTERYDAUERBRIEB)) {
      inverter.dauerbetrieb = doc[BATTERYDAUERBRIEB];
      if(debugConfig) {         
        Serial.print(BATTERYDAUERBRIEB);
        Serial.println(inverter.dauerbetrieb);         
      }     
    }
    if(doc.containsKey(BATTERYSTART)) {
      t1 = doc[BATTERYSTART];
      if(debugConfig) {         
        Serial.print(BATTERYSTART);
        Serial.println(t1);         
      }     
    }  
    if(doc.containsKey(BATTERYSTOP)) {
      t2 = doc[BATTERYSTOP];
      if(debugConfig) {         
        Serial.print(BATTERYSTOP);
        Serial.println(t2);         
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
  doc[BATTERYDAUERBRIEB] = inverter.dauerbetrieb;
  doc[BATTERYSTART] = t1;
  doc[BATTERYSTOP] = t2;

  File configFile = LITTLEFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }
  if(debugConfig) {
    serializeJson(doc, Serial);
  }
  serializeJson(doc, configFile);
  Serial.println(F("\nKonfiguration wurde erfolgreich gespeichert."));

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
    Serial.print(F("Set config value '"));
    Serial.print(key);
    Serial.print(F("' to '"));
    Serial.print(val);
    Serial.println(F("'; Still has to be saved"));
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
      Serial.print(F("Fuer diesen Konfigwert wurde keine Verarbeitung gefunden: "));
      Serial.println(key);
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
     Serial.println(F("Config file is to large"));
     return NULL;
  }
  //allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  if(debugConfig) {
    Serial.println(buf.get());
  }

  DynamicJsonDocument doc(1024);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Serial.println(F("Failed to parse config file"));
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
     Serial.println(F("Config file is to large"));
     return NULL;
  }
  //allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  if(debugConfig) {
    Serial.println(buf.get());
  }

  DynamicJsonDocument doc(1024);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Serial.println(F("Failed to parse config file"));
  }  

  doc[key]=val;  

  Serial.print("Key: ");
  Serial.print(key);
  Serial.print("; Value: ");
  Serial.println(val);
  
  serializeJson(doc, Serial);
  serializeJson(doc, configFile);
  Serial.println(F("\nKonfiguration wurde erfolgreich gespeichert."));

  configFile.flush();
  configFile.close();  
}

void CFG::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("_webUser: "));
  Serial.println(_webUser);  
  Serial.print(F("_webPass: "));
  Serial.println(_webPass); 
  Serial.print(F("debug: "));
  Serial.println(debug); 
  Serial.print(F("debugSma: "));
  Serial.println(debugSma); 
  Serial.print(F("debugSbms: "));
  Serial.println(debugSbms);   
  Serial.print(F("debugJson: "));
  Serial.println(debugJson); 
  Serial.print(F("debugConfig: "));
  Serial.println(debugConfig);   
  Serial.print(F("debugRelais: "));
  Serial.println(debugRelais);      
  Serial.print(F("debugCharger: "));
  Serial.println(debugCharger); 
  Serial.print(F("debugBattery: "));
  Serial.println(debugBattery); 
  Serial.print(F("debugInverter: "));
  Serial.println(debugInverter);  
  Serial.print(BATTERYDAUERBRIEB);
  Serial.println(inverter.dauerbetrieb);   
}
