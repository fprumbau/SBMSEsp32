#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <Update.h>
#include "ESP32OTA.h"
#include "Vars.h"

//https://github.com/bbx10/WebServer_tng/issues/4 Anpassung ESP32


ESP32OTA::ESP32OTA(bool serial_debug){
  _serial_output = serial_debug;
  _server = NULL;
  _authenticated = false;
}

unsigned long t_start,t_stop;

double calcSpeed(unsigned long ms, size_t len){
    return (double)(len * 125) / (double)(ms * 16);
}

void ESP32OTA::setup(WebServer *server, Vars *vars, const char *path, String username, String password) {
    _server = server;
    _username = username;
    _password = password;

    // handler for the /update form page
    _server->on(path, HTTP_GET, [&](){
      if(_username != "" && _password != "" && !_server->authenticate(_username.c_str(), _password.c_str()))
        return _server->requestAuthentication();

      //get page
      String pageIndex = FPSTR(UPDATE_INDEX);
      pageIndex.replace("{title}",_title);
      pageIndex.replace("{banner}",_banner);
      pageIndex.replace("{build}",_build);
      pageIndex.replace("{branch}",_branch);
      pageIndex.replace("{deviceInfo}",_deviceInfo);
      pageIndex.replace("{footer}",_footer);

      _server->send(200, "text/html", pageIndex);
    });

    // handler for the /update form POST (once file upload finishes)
    _server->on(path, HTTP_POST, [&](){
      if(!_authenticated)
        return _server->requestAuthentication();

      String pageSuccess = FPSTR(UPDATE_SUCCESS);
      pageSuccess.replace("{title}",_title);
      pageSuccess.replace("{banner}",_banner);
      pageSuccess.replace("{build}",_build);
      pageSuccess.replace("{branch}",_branch);
      pageSuccess.replace("{deviceInfo}",_deviceInfo);
      pageSuccess.replace("{footer}",_footer);

      String pageFailed = FPSTR(UPDATE_FAILED);
      pageFailed.replace("{title}",_title);
      pageFailed.replace("{banner}",_banner);
      pageFailed.replace("{build}",_build);
      pageFailed.replace("{branch}",_branch);
      pageFailed.replace("{deviceInfo}",_deviceInfo);
      pageFailed.replace("{footer}",_footer);

      _server->send(200, "text/html", Update.hasError() ? pageFailed : pageSuccess);
      ESP.restart();
    },[&](){
      // handler for the file upload, get's the sketch bytes, and writes
      // them through the Update object
      HTTPUpload& upload = _server->upload();
      if(upload.status == UPLOAD_FILE_START){
        t_start = millis();
        if (_serial_output)
          Serial.setDebugOutput(true);

        _authenticated = (_username == "" || _password == "" || _server->authenticate(_username.c_str(), _password.c_str()));
        if(!_authenticated){
          if (_serial_output)
            Serial.printf("Unauthenticated Update\n");
          return;
        }
        //UDP stoppen, stoert Update sonst
        //vars->udp.stop();
        
        if (_serial_output)
          Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          if (_serial_output) Update.printError(Serial);
        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_WRITE){
        if (_serial_output) Serial.printf(".");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          if (_serial_output) Update.printError(Serial);

        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          if (_serial_output) {
            t_stop = millis();
            Serial.println(ESP.getSdkVersion());
            Serial.print("Time UPLOAD: "); Serial.print((t_stop - t_start) / 1000.0); Serial.println(" sec.");
            Serial.print("Speed UPLOAD: "); Serial.print(calcSpeed(t_stop - t_start, upload.totalSize)); Serial.println(" Kbit/s");
            Serial.printf("Upload Success, Rebooting: %u bytes\n", upload.totalSize);
          }
        } else {
          if (_serial_output) Update.printError(Serial);
        }
        if (_serial_output) Serial.setDebugOutput(false);
      } else if(_authenticated && upload.status == UPLOAD_FILE_ABORTED){
        Update.end();
        if (_serial_output) Serial.println("Update was aborted");
      }
      delay(0);
    }); 
}

void ESP32OTA::setTitle(String title)
{
  _title = title;
}

void ESP32OTA::setBanner(String banner)
{
  _banner = banner;
}

void ESP32OTA::setBuild(String build)
{
  _build = build;
}

void ESP32OTA::setBranch(String branch)
{
  _branch = branch;
}

void ESP32OTA::setDeviceInfo(String deviceInfo)
{
  _deviceInfo = deviceInfo;
}

void ESP32OTA::setFooter(String footer)
{
  _footer = footer;
}

void ESP32OTA::setUpdaterUi(String title,String banner,String build,String branch,String footer)
{
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    _footer = footer;
}

void ESP32OTA::updatePassword(String password)
{
  _password = password;
}
