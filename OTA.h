#ifndef __OTA_H
#define __OTA_H

#include "global.h"
#include <ESPmDNS.h>

class OTA
{
  private:
    String _username = "";
    String _password = "";
    bool _authenticated;

    String _title = "OTA updater";
    String _banner = "OTA updater";
    String _build = "Build : ";
    String _branch = "Branch : ";
    String _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    String _footer = "ESP32 OTA Updater";
    void setup(const char *path, String username, String password);
    
  public:
    volatile bool stopForOTA = false;
    void init(const char* host);

    //setup web UI
    void setUpdaterUi(String title,String banner,String build,String branch,String deviceInfo,String footer);
    //setup web UI , with chip ID auto generated
    void setUpdaterUi(String title,String banner,String build,String branch,String footer);
    bool restartRequired = false; // Set this flag in the callbacks to restart ESP in the main loop
};
#endif
