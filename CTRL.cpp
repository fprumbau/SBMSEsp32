#include "global.h" 


bool CTRL::isUpForSeconds(int seconds) {
  long sinceMillis = seconds * 1000;
  long diff = millis() - sinceMillis;
  return  diff > 0;
}

void CTRL::print() {
  Log.warning(F("--------------------------------"));
  Log.warning(F("Running since: %s" CR), runningSince);
  Log.warning(F("Lüfter aktiv: %T" CR), luefter.isOn());
  Log.warning(F("Temperatur: %s" CR), temp);
  Log.warning(F("Now (millis): %d" CR), millis());
  Log.warning(F("Udp resets: %s" CR), udpResets);
  Log.warning(F("Last status message: %s" CR), lastStatusMsg);
  Log.warning(F("Free Heap: %s" CR), ESP.getFreeHeap());

  Log.warning(F("===== DATA ===== "));
  Log.warning(F("currentdc1power: %s" CR), string1);
  Log.warning(F("currentdc2power: %s" CR), string2);
  Log.warning(F("vorlauf: %s" CR), vorlauf);
  Log.warning(F("ruecklauf: %s" CR), ruecklauf);
  Log.warning(F("pegel: %s" CR), pegel);
}

void CTRL::retrieveData() {
  if(myWifi.connected() && !updater.stopForOTA) {
      WiFiClient client;
      HTTPClient http;
      http.begin(client, F("http://192.168.178.36/data")); //http://monitor2/data
      yield();
      
      int httpCode = http.GET();
      vTaskDelay(8);
      if (httpCode > 0) { //Check for the returning code
      
          String payload = http.getString();
      
          DynamicJsonDocument doc(256);
          deserializeJson(doc, payload); 
      
          yield();
          if(debugCtrl) {
            Log.warning(F("CTRL.retrieveData: SerializeJsonPretty to Serial"));
            serializeJsonPretty(doc, Serial);
          }

          string1 = doc["currentdc1power"];
          if(string1 > 10000 || string1 < -100) {
            string1 = -1;
          }
          string2 = doc["currentdc2power"];
          if(string2 > 10000 || string2 < -100) {
            string2 = -1;
          }       
          vorlauf = doc["vorlauf"];
          ruecklauf = doc["ruecklauf"];
          pegel = doc["pegel"];
          temp = doc["temp"];

          Log.notice(F("CTRL.retrieveData GET: read ok"));
              
      } else {       
        Log.error(F("CTRL.retrieveData: Error sending GET: " CR), http.errorToString(httpCode));
      }    
      http.end();
      yield();      
  }
}
