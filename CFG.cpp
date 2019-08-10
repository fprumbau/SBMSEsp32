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
  if(size>1024) {
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
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Serial.println(F("Failed to parse config file"));
  }

  init();
}

void CFG::init() {
  //Konfigwerte initialisieren 

  if(doc.containsKey("authorization") && doc.containsKey("authorization")) {
    
      String auth = doc["authorization"];
      String vehicleId = doc["vehicleId"];
    
      Serial.print(F("Initialisiere Teslakonfig: authorization:|"));
      Serial.print(auth);
      Serial.print(F("|; vehicleId:|"));
      Serial.print(vehicleId);
      Serial.println(F("|"));
    
      perry.init(auth.c_str(), vehicleId.c_str());
  }

  if(doc.containsKey("authorization") && doc.containsKey("authorization")) {
    //Webzugang
    const char* webUser = doc["webuser"];
    _webUser = new char[strlen(webUser)+1];
    strcpy(_webUser, webUser);
  
    const char* webPass = doc["webpass"];
    _webPass = new char[strlen(webPass)+1];
    strcpy(_webPass, webPass);
  
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
  
  return true;
}

void CFG::set(const String& keyVal) {

  String key = getValue(keyVal, ':', 0);
  String val = getValue(keyVal, ':', 1);
  
  doc[key] = val;
  Serial.print(F("Set config value '"));
  Serial.print(key);
  Serial.print(F("' to '"));
  Serial.print(val);
  Serial.println(F("'; Still has to be saved"));
  
  //Konfiginitialisierung durchfuehren
  init();
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
}
