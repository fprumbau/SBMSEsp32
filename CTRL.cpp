#include "global.h" 


bool CTRL::isUpForSeconds(int seconds) {
  //falls das Geraet zu lange laeuft ( >50d 70min ) muss der Code 'schlauer' sein
  //https://www.norwegiancreations.com/2018/10/arduino-tutorial-avoiding-the-overflow-issue-when-using-millis-and-micros/
  if((unsigned long)(millis() - time_now) > seconds) {
    time_now = millis();
    return true;
  }
  return false;
}

void CTRL::print() {
  Serial.println(F("--------------------------------"));
  Serial.print(F("Running since: "));
  Serial.println(runningSince);
  Serial.print(F("Lüfter aktiv: "));
  Serial.println(luefter.isOn());
  Serial.print(F("Temperatur: "));
  Serial.println(temp);  
  Serial.print(F("Now (millis): "));
  Serial.println(millis());        
  Serial.print(F("Udp resets: "));
  Serial.println(udpResets);  
  Serial.print(F("Last status message: "));
  Serial.println(lastStatusMsg );
  Serial.print(F("Free Heap: "));
  Serial.println(ESP.getFreeHeap());  
  Serial.print(F("===== DATA ===== "));
  Serial.print(F("currentdc1power: "));
  Serial.println(string1);
  Serial.print(F("currentdc2power: "));
  Serial.println(string2);
  Serial.print(F("vorlauf: "));
  Serial.println(vorlauf);  
  Serial.print(F("ruecklauf: "));
  Serial.println(ruecklauf);    
  Serial.print(F("pegel: "));
  Serial.println(pegel);
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
            Serial.println(F("CTRL.retrieveData: SerializeJsonPretty to Serial"));
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

          Serial.println(F("CTRL.retrieveData GET: read ok"));
              
      } else {
        
        Serial.print(F("CTRL.retrieveData: Error sending GET: "));
        Serial.println(http.errorToString(httpCode));

      }    
      http.end();
      yield();      
  }
}
