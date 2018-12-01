#ifndef __ESP32_OTA_H
#define __ESP32_OTA_H

#include "Vars.h"
#include "UpdaterUI.h"

class WebServer;

class ESP32OTA
{
  private:
    bool _serial_output;
    WebServer *_server;
    String _username = "";
    String _password = "";
    bool _authenticated;

    String _title = "OTA updater";
    String _banner = "OTA updater";
    String _build = "Build : ";
    String _branch = "Branch : ";
    String _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    String _footer = "ESP32 OTA Updater";

  public:
    ESP32OTA(bool serial_debug=true);

    void setup(WebServer *server, Vars *vars, const char * path, String username, String password);

    void setTitle(String title);
    void setBanner(String banner);
    void setBuild(String build);
    void setBranch(String branch);
    void setDeviceInfo(String deviceInfo);
    void setFooter(String footer);

    //setup web UI
    void setUpdaterUi(String title,String banner,String build,String branch,String deviceInfo,String footer);
    //setup web UI , with chip ID auto generated
    void setUpdaterUi(String title,String banner,String build,String branch,String footer);
    //update passsword
    void updatePassword(String password);
};
#endif
