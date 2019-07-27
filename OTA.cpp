#include <Update.h>

#include "html.h"
#include "global.h"

//https://github.com/bbx10/WebServer_tng/issues/4 Anpassung ESP32

/**
 * - Wenn die Lader angeschaltet sind, aber der Ladestrom < 1A betr&auml;gt, k&ouml;nnen die L&uuml;fter abgeschaltet werden / bleiben
 */

void OTA::init(const char* host) {
  //OTA is possible only with 4mb memory
  long flashSize = ESP.getFlashChipSize();
  Serial.print("Flash Size: ");
  Serial.println(flashSize);
  if(flashSize > 4000000) {
     //set web UI
     MDNS.begin(host);
     MDNS.addService("http", "tcp", 80);
     Serial.printf("\n\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
     String _version = "Build : ";
     _version += VERSION;
     updater.setUpdaterUi("Title", _version, "SBMS120 Solar Charger", "Branch : master", String(changelog));
     //Optional: Authentifizieren
     //updater.setup("/update", "admin", "Go8319!");
     updater.setup("/update", "", "");
  } else {
     Serial.println(F("Flash OTA programming only possible with 4Mb Flash size!!!"));
  }
}

double calcSpeed(unsigned long ms, size_t len){
    return (double)(len * 125) / (double)(ms * 16);
}

void OTA::setup(const char *path, String username, String password) {

    _username = username;
    _password = password;
    uint32_t t_start,t_stop;
    size_t fileSize;
    uint16_t ct = 0;
    

    // handler for the /update form page
    server.on(path, HTTP_GET, [&](AsyncWebServerRequest *request){
               
        String pageIndex = String(update);
        pageIndex.replace("{title}",_title);
        pageIndex.replace("{banner}",_banner);
        pageIndex.replace("{build}",_build);
        pageIndex.replace("{branch}",_branch);
        pageIndex.replace("{deviceInfo}",_deviceInfo);
        pageIndex.replace("{footer}",_footer);
      
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", pageIndex);
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // handler for the /update form POST (once file upload finishes)
   server.on(path, HTTP_POST, [&](AsyncWebServerRequest *request){
        // the request handler is triggered after the upload has finished... 
        // create the response, add header, and send response
        String pageIndex = String(update);
        pageIndex.replace("{title}",_title);
        if(Update.hasError()){
          pageIndex.replace("{banner}",F("<b><font color=red>Update gescheitert</font></b>"));
        } else {
          pageIndex.replace("{banner}",F("<b><font color=green>Update erfolgreich</font></b>"));
          pageIndex.replace("{redirect}", "redirect=true;");
        }
        pageIndex.replace("{build}",_build);
        pageIndex.replace("{branch}",_branch);
        pageIndex.replace("{deviceInfo}",_deviceInfo);
        pageIndex.replace("{footer}",_footer);
      
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", pageIndex);
      response->addHeader("Connection", "close");
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    },[&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
      //Upload handler chunks in data     
      if(!index){ // if index == 0 then this is the first frame of data
        stopForOTA = true; //stoppt alle Aktionen im loop()

        delay(3000); //geht das hier?? unsicher!!!
        udp.stop(); //koennte helfen
        
        Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.setDebugOutput(true);
        t_start = millis();
        fileSize = len;
        
        // calculate sketch space required for the update; 1048576
        uint32_t maxSketchSpace = (1248576 - 0x1000) & 0xFFFFF000;
        Serial.print("maxSketchSpace: ");
        Serial.println(maxSketchSpace);
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
        //Update.runAsync(true); // tell the updaterClass to run in async mode (nicht da fuer ESP32)
      } else {
        ct++;
        if(ct%70==0) Serial.println("");
        Serial.print(".");
        fileSize += len;
      }
      //Write chunked data to the free sketch space
      if(Update.write(data, len) != len){
          Update.printError(Serial);
      }
      
      if(final){ // if the final flag is set then this is the last frame of data
        if(Update.end(true)){ //true to set the size to the current progress
            t_stop = millis();
            Serial.print(F("\nTime UPLOAD: ")); Serial.print((t_stop - t_start) / 1000.0); Serial.println(" sec.");
            Serial.print(F("Speed UPLOAD: ")); Serial.print(calcSpeed(t_stop - t_start, fileSize)); Serial.println(" Kbit/s");
            Serial.printf("Upload Success, Rebooting: %u bytes\n", fileSize);
            restartRequired = true;  // Tell the main loop to restart the ESP
        } else {
            Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
  });

  //Hmmh not found, gehoert eigentlich nicht hier hin
  server.onNotFound([](AsyncWebServerRequest *request){
      Serial.printf("NOT_FOUND: ");
      if(request->method() == HTTP_GET)
        Serial.printf("GET");
      else if(request->method() == HTTP_POST)
        Serial.printf("POST");
      else if(request->method() == HTTP_DELETE)
        Serial.printf("DELETE");
      else if(request->method() == HTTP_PUT)
        Serial.printf("PUT");
      else if(request->method() == HTTP_PATCH)
        Serial.printf("PATCH");
      else if(request->method() == HTTP_HEAD)
        Serial.printf("HEAD");
      else if(request->method() == HTTP_OPTIONS)
        Serial.printf("OPTIONS");
      else
        Serial.printf("UNKNOWN");
      Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());
  
      if(request->contentLength()){
        Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
        Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
      }
  
      int headers = request->headers();
      int i;
      for(i=0;i<headers;i++){
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
      }
  
      int params = request->params();
      for(i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isFile()){
          Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if(p->isPost()){
          Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
          Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }

    request->send(404);
  });  

}

void OTA::setUpdaterUi(String title,String banner,String build,String branch,String footer) {
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    _footer = footer;
}
