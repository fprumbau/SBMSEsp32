#include "config.h"

#include "global.h"

bool CFG::load() {

  if(!SPIFFS.begin()) {
    return false;
  }
  File configFile = SPIFFS.open("/config.json", "r");
  if(!configFile){
    return false;
  }
  size_t size = configFile.size();
  if(size>1024) {
     Serial.println(F("Config file is to large"));
     return false;
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
    return _configRead;
  }

  init();
  
  _configRead = true;
  
  return _configRead;
}

void CFG::init() {
  //Konfigwerte initialisieren 

  String auth = doc["authorization"];
  String vehicleId = doc["vehicleId"];
  perry.init(auth.c_str(), vehicleId.c_str());

  //Lese andere Konfigwerte fuer global.h
  teslaCtrlActive = doc["teslaCtrlActive"];
  Serial.print("teslaCtrlActive=");
  Serial.println(teslaCtrlActive);  
}

bool CFG::save() {
  if(!SPIFFS.begin()) {
    return false;
  }
  if(!_configRead) {
    wc.sendClients(F("Kann Konfig nicht schreiben, da sie nie gelesen wurde"));
    return false;
  }
  
  //doc["vehicleId"] = "50...15";
  //doc["authorization"] = "Bearer 81..40";
  
  doc["teslaCtrlActive"] = teslaCtrlActive;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }

  serializeJson(doc, configFile);
  wc.sendClients(F("Konfiguration wurde erfolgreich gespeichert."));
  
  return true;
}

void CFG::set(const String& keyVal) {

  String key = getValue(keyVal, ':', 0);
  String val = getValue(keyVal, ':', 1);
  
  doc[key] = val;
  Serial.print("Set config value ");
  Serial.print(key);
  Serial.print(" to ");
  Serial.print(val);
  Serial.println("; Still has to be saved");
  
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
