#ifndef __ESP32_OTA_H
#define __ESP32_OTA_H

#include "Vars.h"
#include "UpdaterUI.h"

class WebServer;

class ESP32OTA
{
  private:
    WebServer *_server;
    String _username = "";
    String _password = "";
    bool _authenticated;

    Vars& vars; //Global definierte Variablen
    String _title = "OTA updater";
    String _banner = "OTA updater";
    String _build = "Build : ";
    String _branch = "Branch : ";
    String _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    String _footer = "ESP32 OTA Updater";

  public:
    ESP32OTA(Vars &vars);

    void setup(WebServer *server, const char *path, String username, String password);

    //setup web UI
    void setUpdaterUi(String title,String banner,String build,String branch,String deviceInfo,String footer);
    //setup web UI , with chip ID auto generated
    void setUpdaterUi(String title,String banner,String build,String branch,String footer);
};
#endif
