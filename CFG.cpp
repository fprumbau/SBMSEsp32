#include "config.h"

#include "global.h"

void CFG::load() {

  if(!SPIFFS.begin()) {
    return;
  }
  File configFile = SPIFFS.open("/config.json", "r");
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
  if(debug) Serial.println(buf.get());

  DynamicJsonDocument doc(1024);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Serial.println(F("Failed to parse config file"));
  }

  if(doc.containsKey("authorization") && doc.containsKey("vehicleId")) {
    
      const char* auth = doc["authorization"];
      const char* vehicleId = doc["vehicleId"];

      if(debugConfig) {
        Serial.print(F("Initialisiere Teslakonfig: authorization:|"));
        Serial.print(auth);
        Serial.print(F("|; vehicleId:|"));
        Serial.print(vehicleId);
        Serial.println(F("|"));
      }
    
      perry.vehicleId(vehicleId);
      perry.authorization(auth);
  }

  if(doc.containsKey("webuser") && doc.containsKey("webpass")) {
    //Webzugang
    const char* webUser = doc["webuser"];
    _webUser = new char[strlen(webUser)+1];
    strcpy(_webUser, webUser); 
  
    const char* webPass = doc["webpass"];
    _webPass = new char[strlen(webPass)+1];
    strcpy(_webPass, webPass);

    if(debugConfig) {
      Serial.print(F("Initialisiere User/Pw: user:|"));
      Serial.print(_webUser);
      Serial.print(F("|; password:|"));
      Serial.print(_webPass);
      Serial.println(F("|"));
    }
  
    //Lese andere Konfigwerte fuer global.h
    teslaCtrlActive = doc["teslaCtrlActive"];
    if(debug) {
      Serial.print("teslaCtrlActive=");
      Serial.println(teslaCtrlActive);  
    }
  }
}

bool CFG::save() {
  
  if(!SPIFFS.begin()) {
    return false;
  }

  DynamicJsonDocument doc(1024);
  
  doc["vehicleId"] = perry.vehicleId();
  doc["authorization"] = perry.authorization();
  doc["teslaCtrlActive"] = teslaCtrlActive;
  doc["webUser"] = _webUser;
  doc["webPass"] = _webPass;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }
  serializeJson(doc, Serial);
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

  if(keyStr == "vehicleId") {
    perry.vehicleId(val);
  } else if(keyStr == "authorization") {
    perry.authorization(val);
  } else if(keyStr == "webUser") {
    _webUser = new char[strlen(val)+1];
    strcpy(_webUser, val);    
  } else if(keyStr == "webPass") {
    _webPass = new char[strlen(val)+1];
    strcpy(_webPass, val);
  } else if(keyStr == "teslaCtrlActive") {
    if(strcmp(val,"1") || strcmp(val,"true")) {
      teslaCtrlActive=true;
    } else {
      teslaCtrlActive=false;
    }
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
void CFG::print() {
  Serial.print(F("_webUser: "));
  Serial.println(_webUser);  
  Serial.print(F("_webPass: "));
  Serial.println(_webPass); 
  Serial.print(F("debug: "));
  Serial.println(debug); 
  Serial.print(F("debug2: "));
  Serial.println(debug2);   
  Serial.print(F("debugJson: "));
  Serial.println(debugJson); 
  Serial.print(F("debugRelais: "));
  Serial.println(debugRelais);    
  Serial.print(F("debugSma: "));
  Serial.println(debugSma); 
  Serial.print(F("debugSbms: "));
  Serial.println(debugSbms);   
  Serial.print(F("debugCharger: "));
  Serial.println(debugCharger); 
  Serial.print(F("debugBattery: "));
  Serial.println(debugBattery); 
  Serial.print(F("debugInverter: "));
  Serial.println(debugInverter);  
}
