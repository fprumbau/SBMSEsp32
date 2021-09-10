#include "Tesla.h"

#include "global.h"

//DOKU
//https://tesla-api.timdorr.com/api-basics/authentication

int Tesla::wakeup() {

    HTTPClient http;

    beginRequest(&http, _wakeup_url);
    yield();
    
    int rc = http.POST("");
    Log.noticeln(F("Trying to issue wakeup: %s"), rc);
    yield();
 
    if(rc>0) {
        if(debugTesla) {
          Log.noticeln(_wakeup_url);
          String response = http.getString();    
          Log.noticeln(response.c_str());  
          wc.sendClients(response.c_str());
        }
        //200 == OK,
        //401 == Bearer Token abgelaufen oder nicht richtig

    } else {
        Log.errorln(F("Error sending wakeup POST"), http.errorToString(rc));
        wc.sendClients(http.errorToString(rc).c_str());
    }
    yield();
    http.end();
    yield();
    return rc;
}

int Tesla::authorize(const char* password) {
    Log.noticeln(F("Re-authorization mit password >>%s<<"), password);

    WiFiClient client;
    HTTPClient http;
    http.begin(client, _auth_url);
    http.addHeader(_hd_user_agent, _user_agent);
    http.addHeader(_hd_app_agent, _tesla_user_agent);
    http.addHeader(_hd_content_type, _json_header);
    
    yield();

    //Prepare Requestdata
    DynamicJsonDocument doc(256);
    doc["email"]="fprumbau@gmail.com";
    doc["password"]=password;
    doc["client_id"]=TESLA_CLIENT_ID;
    doc["client_secret"]=TESLA_CLIENT_SECRET;
    doc["grant_type"]="password";  //alternativ: refresh_token, dann STATT password refresh_token="alter token", der verliert dann seine Gueltigkeit!
    String reqData = String((char *)0);
    reqData.reserve(256);
    serializeJson(doc, reqData);
    Log.noticeln(reqData.c_str());
    
    int rc = http.POST(reqData);
    Log.noticeln(F("Trying to renew bearer token: %s"), rc);
    yield();
 
    if(rc == 200) {
      
        Log.noticeln(_auth_url);
        String response = http.getString();    
        Log.notice(response.c_str());  
        wc.sendClients(response.c_str());
        DynamicJsonDocument doc(256); 
        deserializeJson(doc, response);

        //"Bearer TOKEN"
        String accessToken = doc["access_token"];
        String rt = String((char *)0);
        rt.reserve(128);
        rt = "Bearer ";
        rt += accessToken;
        _authorization = new char[rt.length()+1];
        rt.toCharArray(_authorization, rt.length()+1); 
               
        yield();
        config.save();

        Log.warningln(response.c_str());  
        Log.warningln(F("Authorization: %s"), _authorization);
        Log.warningln(F("VehicleId: %s\nDer AuthToken wurde erneuert und gespeichert"), _vehicle_id);               
        /*
        {
          "access_token": "abc123",
          "token_type": "bearer",
          "expires_in": 3888000,  //45d 
          "refresh_token": "cba321",
          "created_at": 1538359034
        }
        */          

        //200 == OK,
        //401 == Bearer Token abgelaufen oder nicht richtig

    } else {
        Log.warningln(F("Error sending wakeup POST"), http.errorToString(rc));        
    }
    yield();
    http.end();
    yield();
    return rc;
}

int Tesla::readChargeState() {

  HTTPClient http;

  beginRequest(&http, _get_charge_state_url);
  yield();
  
  int rc = http.GET();
  Log.warningln(F("Trying to issue read charge state: "), rc);
  yield();
  
  if(rc>0) {
      Log.warningln(_get_charge_state_url);
      String response = http.getString();    
      Log.warningln(response.c_str());  
      wc.sendClients(response.c_str());
      yield();

      //Update
      if(rc==200) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);
        JsonObject obj = doc.as<JsonObject>();
        JsonObject resp = obj["response"].as<JsonObject>();     
        
        _chargeRate = resp["charger_rate"];
        _chargerPhases = resp["charger_phases"];
        _chargerActualCurrent = resp["charger_actual_current"];
        _chargerPower = resp["charger_power"];
        _chargeLimitSoc = resp["charge_limit_soc"];    
        _batteryLevel = resp["battery_level"];  
        _chargerVoltage = resp["charger_voltage"];
        String cState = resp["charging_state"]; //Complete|Charging|Stopped|Disconnected|Starting
        _chargingState = cState;
  
        _hasUpdate = true;
  
        //"charge_rate":10.9            10.9 km/h
        //"charger_phases":1            eine Phase
        //"charger_pilot_current":13    13A
        //"charger_actual_current":13   13A
        //"charger_power":3             3kw
        //"charge_limit_soc":90         90% charge limit
        //"battery_level":61            61%
        
        //200 == OK,
        //401 == Bearer Token abgelaufen oder nicht richtig
        //408 == 'vehicle not available' => Wakup
      }
  } else {
      Log.errorln(F("Error sending GET charge state; rc=%s\n%s"), rc, http.errorToString(rc));
  }
  
  yield();
  http.end();
  yield();
  return rc;
}
    
int Tesla::startCharge() {

    HTTPClient http;
      
    yield();
    beginRequest(&http, _charge_start_url);    
    yield();
    int rc = http.POST("");
    Log.warningln(F("Trying to issue charge start: %s"), rc);
    
    if(rc>0) {  
        if(debugTesla) {
          String response = http.getString();                        
          Log.warningln(_charge_start_url);
          Log.warningln(response.c_str());
          wc.sendClients(response.c_str());
        }
    } else {
        Log.warningln(F("Error sending POST charge start %s"), http.errorToString(rc));
    }
    yield();
    http.end();
    yield();
    return rc;   
}

int Tesla::stopCharge() {

    HTTPClient http;

    yield();
    beginRequest(&http, _charge_stop_url);
    yield();
    
    int rc = http.POST("");
    Log.warningln(F("Trying to issue charge stop: %s"), rc);
    yield();

    if(rc>0) {  
        Log.warningln(_charge_stop_url);    
        String response = http.getString(); 
        Log.warningln(response.c_str()); 
        wc.sendClients(response.c_str());
    } else {
        Log.warningln(F("Error sending POST charge stop\n%s"), http.errorToString(rc));
    }
    yield();
    http.end();
    yield();
    return rc;    
}

int Tesla::setChargeLimit(int percent) {

  HTTPClient http;

  yield();
  beginRequest(&http, _set_charge_limit_url);
  yield();

  DynamicJsonDocument doc(32);
  doc["percent"]=percent;
  String reqData = String((char *)0);
  reqData.reserve(32);
  serializeJson(doc, reqData);
  Log.noticeln(reqData.c_str());
  yield();

  int rc = http.POST(reqData);
  yield();
  Log.warningln(F("Trying to issue set charge limit: %s"), rc);
  
  if(rc>0) {  
      if(debugTesla) {
        Log.warningln(_set_charge_limit_url);
        String response = http.getString();                        
        Log.warningln(response.c_str());  
        wc.sendClients(response.c_str());
      }
  } else {
      Log.errorln(F("Error sending POST charge limit\n%s"), http.errorToString(rc));
  }  
  yield();
  http.end();
  yield();
  return rc;
}

void Tesla::authorization(const char* auth) {
  
  _authorization = new char[strlen(auth)+1];
  strcpy(_authorization, auth);
}

void Tesla::vehicleId(const char* vehicleid) {

  _vehicle_id = new char[strlen(vehicleid)+1];
  strcpy(_vehicle_id, vehicleid);
  
  //URLS festlegen
  String vehicleBaseUrl((char *)0);
  vehicleBaseUrl.reserve(71); //genauer Laenge 68 + 3 (Reserve)
  vehicleBaseUrl += _api_base_url;
  vehicleBaseUrl += _vehicle_id;
  _vehicle_base_url = new char[vehicleBaseUrl.length()+1];
  strcpy(_vehicle_base_url, vehicleBaseUrl.c_str());
  
  String chargeLimitUrl((char *)0);
  chargeLimitUrl.reserve(106); //genaue Laenge 103 + 3 (Reserve)
  chargeLimitUrl += _vehicle_base_url;
  chargeLimitUrl += _charge_limit;
  _set_charge_limit_url = new char[chargeLimitUrl.length()+1];
  strcpy(_set_charge_limit_url, chargeLimitUrl.c_str());

  String chargeStartUrl((char *)0);
  chargeStartUrl.reserve(91); //genaue Laenge 88 + 3 (Reserve)
  chargeStartUrl += _vehicle_base_url;
  chargeStartUrl += _charge_start;
  _charge_start_url = new char[chargeStartUrl.length()+1];
  strcpy(_charge_start_url, chargeStartUrl.c_str());   

  String chargeStopUrl((char *)0);
  chargeStopUrl.reserve(90); //genaue Laenge 87 + 3 (Reserve)
  chargeStopUrl += _vehicle_base_url;
  chargeStopUrl += _charge_stop;
  _charge_stop_url = new char[chargeStopUrl.length()+1];
  strcpy(_charge_stop_url, chargeStopUrl.c_str());

  String chargeStateUrl((char *)0); 
  chargeStateUrl.reserve(96); //genaue Laenge 93 + 3 (Reserve)
  chargeStateUrl += _vehicle_base_url;
  chargeStateUrl += _charge_state;
  _get_charge_state_url = new char[chargeStateUrl.length()+1];
  strcpy(_get_charge_state_url, chargeStateUrl.c_str());

  String wakeupUrl((char *)0);
  wakeupUrl.reserve(78); //genaue Laenge 75 + 3 (Reserve)
  wakeupUrl += _vehicle_base_url;
  wakeupUrl += _wakeup;
  _wakeup_url = new char[wakeupUrl.length()+1];
  strcpy(_wakeup_url, wakeupUrl.c_str());

  String authUrl((char *)0);
  authUrl.reserve(50); //spaeter
  authUrl += F("https://owner-api.teslamotors.com");
  authUrl += _auth;
  _auth_url = new char[authUrl.length()+1];
  strcpy(_auth_url, authUrl.c_str());
}

void Tesla::print() {
  Log.warningln(F("--------------------------------"));
  Log.warningln(_set_charge_limit_url);
  Log.warningln(_charge_start_url);
  Log.warningln(_charge_stop_url);
  Log.warningln(_get_charge_state_url);
  Log.warningln(_wakeup_url);
  Log.warningln(_auth_url);
  Log.warningln(F("Has update: %T"), _hasUpdate);
  Log.warningln(F("Chargerate: %s"), _chargeRate);
  Log.warningln(F("ChargePhases: %s"), _chargerPhases);
  Log.warningln(F("ChargerActualCurrent: %s"), _chargerActualCurrent);
  Log.warningln(F("ChargerPower: %s"), _chargerPower);
  Log.warningln(F("BatteryLevel: %s"), _batteryLevel);
  Log.warningln(F("ChargeLimitSoc: %s"), _chargeLimitSoc);
  Log.warningln(F("ChargingState: %s"), _chargingState);
  Log.warningln(F("Ladespannung: %s"), _chargerVoltage);
  Log.warningln(F("VehicleId: %s"), _vehicle_id);
  Log.warningln(F("Bearer Token: %s"), _authorization);
} 

const char* Tesla::status() {
  String status = String((char*)0);
  status.reserve(256);

  status+=F("<b>Chargerate:</b> ");
  status+=_chargeRate;
  status+=F("<br><b>ChargePhases:</b> ");
  status+=_chargerPhases;
  status+=F("<br><b>ChargerActualCurrent:</b> ");
  status+=_chargerActualCurrent;
  status+=F("<br><b>ChargerPower:</b> ");
  status+=_chargerPower;
  status+=F("<br><b>BatteryLevel:</b> ");
  status+=_batteryLevel;
  status+=F("<br><b>ChargeLimitSoc:</b> ");
  status+=_chargeLimitSoc;
  status+=F("<br><b>ChargingState:</b> ");
  status+=_chargingState;
  status+=F("<br><b>Spannung:</b> ");
  status+=_chargerVoltage;

  return status.c_str();
}

void Tesla::beginRequest(HTTPClient *client, char *url) {
    client->begin(url);
    if(debugTesla) {
      Log.warningln(F("Is connected: %s"), client->connected());      
      Log.warningln(F("Setting Header: %s to %s"), _hd_user_agent, _user_agent);  
      Log.warningln(F("Setting Header: %s to %s"), _hd_app_agent, _tesla_user_agent); 
      Log.warningln(F("Setting Header: %s to %s"), _hd_content_type, _json_header);           
    }
    client->addHeader(_hd_user_agent, _user_agent);
    client->addHeader(_hd_app_agent, _tesla_user_agent);
    client->addHeader(_hd_content_type, _json_header);
    client->addHeader(_hd_authorization, _authorization);   
}

bool Tesla::isCharging() {
    //Complete|Charging|Stopped|Disconnected
    if(_chargingState.equals("Charging") || _chargingState.equals("Starting")) {
      return true;
    }
    return false;
}

void Tesla::setCharging(bool charging) {
  if(charging) {
    _chargingState = F("Charging");
  } else {
    _chargingState = F("Stopped");
  }
}

int Tesla::getSoC() {
  return _batteryLevel;
}

bool Tesla::hasUpdate() {
  return _hasUpdate;
}

void Tesla::reset() {
  _hasUpdate = false;
}

const char* Tesla::vehicleId() {
  return _vehicle_id;
}

const char* Tesla::authorization() {
  return _authorization;
}
