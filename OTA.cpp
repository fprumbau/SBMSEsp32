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
  Log.warningln(F("Flash Size: %d"), flashSize);
  if(flashSize > 4000000) {
     //set web UI
     MDNS.begin(host);
     MDNS.addService("http", "tcp", 80);
     Log.warningln(F("\n\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n"), host);
     String _version = F("Build : ");
     _version += VERSION;
     updater.setUpdaterUi("Title", _version, "SBMS120 Solar Charger", "Branch : master", String(changelog));
     //Optional: Authentifizieren
     //updater.setup("/update", "admin", "Go8319!");
     updater.setup("/update", "", "");
  } else {
     Log.warningln(F("Flash OTA programming only possible with 4Mb Flash size!!!"));
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
          pageIndex.replace(F("{banner}"),F("<b><font color=red>Update gescheitert</font></b>"));
        } else {
          pageIndex.replace(F("{banner}"),F("<b><font color=green>Update erfolgreich</font></b>"));
          pageIndex.replace(F("{redirect}"), F("redirect=true;"));
        }
        pageIndex.replace(F("{build}"),_build);
        pageIndex.replace(F("{branch}"),_branch);
        pageIndex.replace(F("{deviceInfo}"),_deviceInfo);
        pageIndex.replace(F("{footer}"),_footer);
      
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", pageIndex);
      response->addHeader(F("Connection"), F("close"));
      response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
      request->send(response);
    },[&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
      //Upload handler chunks in data     
      if(!index){ // if index == 0 then this is the first frame of data
        stopForOTA = true; //stoppt alle Aktionen im loop()

        yield();
        delay(1500); //geht das hier?? unsicher!!!; 0.9.9.89: -1s
        udp.close(); //0.9.9.98 statt stop()
        
        Log.warningln("UploadStart: %s", filename.c_str());
        t_start = millis();
        fileSize = len;
        
        // calculate sketch space required for the update; 1048576
        // 1248576 auf 1348576 erhoeht, 1.0.17 reverted
        uint32_t maxSketchSpace = (1248576 - 0x1000) & 0xFFFFF000;
        Log.warningln(F("maxSketchSpace: %s"), maxSketchSpace);
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
        //Update.runAsync(true); // tell the updaterClass to run in async mode (nicht da fuer ESP32)
      } else {
        ct++;
        if(ct%70==0) Log.warningln("");
        Log.warning(".");
        fileSize += len;
      }
      //Write chunked data to the free sketch space
      if(Update.write(data, len) != len){
          Update.printError(Serial);
      }
      
      if(final){ // if the final flag is set then this is the last frame of data
        if(Update.end(true)){ //true to set the size to the current progress
            t_stop = millis();
            Log.warningln(F("\nTime UPLOAD: %d sec. Speed UPLOAD:  %dKbit/s; Upload Success, Rebooting: %d bytes"),(t_stop - t_start) / 1000.0, calcSpeed(t_stop - t_start, fileSize), fileSize); 
            restartRequired = true;  // Tell the main loop to restart the ESP
        } else {
            Update.printError(Serial);
        }
      }
  });

  //Hmmh not found, gehoert eigentlich nicht hier hin
  server.onNotFound([](AsyncWebServerRequest *request){
      Log.warning(F("NOT_FOUND: "));
      if(request->method() == HTTP_GET)
        Log.warning(F("GET"));
      else if(request->method() == HTTP_POST)
        Log.warning(F("POST"));
      else if(request->method() == HTTP_DELETE)
        Log.warning(F("DELETE"));
      else if(request->method() == HTTP_PUT)
        Log.warning(F("PUT"));
      else if(request->method() == HTTP_PATCH)
        Log.warning(F("PATCH"));
      else if(request->method() == HTTP_HEAD)
        Log.warning(F("HEAD"));
      else if(request->method() == HTTP_OPTIONS)
        Log.warning(F("OPTIONS"));
      else
        Log.warning(F("UNKNOWN"));
      Log.warningln(" http://%s%s", request->host().c_str(), request->url().c_str());
  
      if(request->contentLength()){
        Log.warningln("_CONTENT_TYPE: %s\n_CONTENT_LENGTH: %u", request->contentType().c_str(), request->contentLength());
      }
  
      int headers = request->headers();
      int i;
      for(i=0;i<headers;i++){
        AsyncWebHeader* h = request->getHeader(i);
        Log.warningln("_HEADER[%s]: %s", h->name().c_str(), h->value().c_str());
      }
  
      int params = request->params();
      for(i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isFile()){
          Log.warningln("_FILE[%s]: %s, size: %u", p->name().c_str(), p->value().c_str(), p->size());
        } else if(p->isPost()){
          Log.warningln("_POST[%s]: %s", p->name().c_str(), p->value().c_str());
        } else {
          Log.warningln("_GET[%s]: %s", p->name().c_str(), p->value().c_str());
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
