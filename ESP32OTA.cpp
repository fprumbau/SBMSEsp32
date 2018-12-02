#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <Update.h>
#include "global.h"

//https://github.com/bbx10/WebServer_tng/issues/4 Anpassung ESP32

double calcSpeed(unsigned long ms, size_t len){
    return (double)(len * 125) / (double)(ms * 16);
}

void ESP32OTA::setup(const char *path, String username, String password) {
    _username = username;
    _password = password;

    // handler for the /update form page
    server.on(path, HTTP_GET, [&](){
        if(_username != "" && _password != "" && !server.authenticate(_username.c_str(), _password.c_str())) {
          return server.requestAuthentication();
        }
        String pageIndex = FPSTR(UPDATE_INDEX);
        pageIndex.replace("{title}",_title);
        pageIndex.replace("{banner}",_banner);
        pageIndex.replace("{build}",_build);
        pageIndex.replace("{branch}",_branch);
        pageIndex.replace("{deviceInfo}",_deviceInfo);
        pageIndex.replace("{footer}",_footer);
        server.send(200, "text/html", pageIndex);
    });

    // handler for the /update form POST (once file upload finishes)
    server.on(path, HTTP_POST, [&](){
        if(!_authenticated) {
          return server.requestAuthentication();
        }
        String pageIndex = FPSTR(UPDATE_INDEX);
        pageIndex.replace("{title}",_title);
        pageIndex.replace("{banner}",_banner);
        pageIndex.replace("{build}",_build);
        pageIndex.replace("{branch}",_branch);
        pageIndex.replace("{deviceInfo}",_deviceInfo);
        pageIndex.replace("{footer}",_footer);
        server.send(200, "text/html", pageIndex);
        delay(1000);
        ESP.restart();
    },[&](){
      // handler for the file upload, get's the sketch bytes, and writes them through the Update object
      HTTPUpload& upload = server.upload();
      unsigned long t_start,t_stop;
      if(upload.status == UPLOAD_FILE_START){
        t_start = millis();
        Serial.setDebugOutput(true); 

        _authenticated = (_username == "" || _password == "" || server.authenticate(_username.c_str(), _password.c_str()));
        if(!_authenticated){
          Serial.printf("Unauthenticated Update\n");
          return;
        }
        //UDP stoppen, stoert Update sonst
        vars.udp.stop();
        
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (1048576 - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_WRITE){
        //if(vars.debug2) Serial.printf(".");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
           Update.printError(Serial);
        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          t_stop = millis();
          Serial.println(ESP.getSdkVersion());
          Serial.print("Time UPLOAD: "); Serial.print((t_stop - t_start) / 1000.0); Serial.println(" sec.");
          Serial.print("Speed UPLOAD: "); Serial.print(calcSpeed(t_stop - t_start, upload.totalSize)); Serial.println(" Kbit/s");
          Serial.printf("Upload Success, Rebooting: %u bytes\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      } else if(_authenticated && upload.status == UPLOAD_FILE_ABORTED){
        Update.end();
        Serial.println("Update was aborted");
      }
      yield();
    }); 
}

void ESP32OTA::setUpdaterUi(String title,String banner,String build,String branch,String footer) {
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    _footer = footer;
}
