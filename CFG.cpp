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
  //allocate a buffer to store contetns of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  if(debug) Serial.println(buf.get());
  
  StaticJsonDocument<1024> doc;
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Serial.println(F("Failed to parse config file"));
    return false;
  }

  String auth = doc["authorization"];
  String vehicleId = doc["vehicleId"];
  perry.init(auth.c_str(), vehicleId.c_str());
  
  return true;
}

bool CFG::save() {
  if(!SPIFFS.begin()) {
    return false;
  }

  StaticJsonDocument<1024> doc;
  
  //doc["vehicleId"] = "50...15";
  //doc["authorization"] = "Bearer 81..40";

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}
