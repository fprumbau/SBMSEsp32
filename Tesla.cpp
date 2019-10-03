#include "Tesla.h"

#include "global.h"

//DOKU
//https://tesla-api.timdorr.com/api-basics/authentication

int Tesla::wakeup() {

    HTTPClient http;

    beginRequest(&http, _wakeup_url);
    yield();
    
    int rc = http.POST("");
    Serial.print(F("Trying to issue wakeup: "));
    Serial.println(rc);
    yield();
 
    if(rc>0) {
        if(debugTesla) {
          Serial.println(_wakeup_url);
          String response = http.getString();    
          Serial.println(response);  
          wc.sendClients(response.c_str());
        }
        //200 == OK,
        //401 == Bearer Token abgelaufen oder nicht richtig

    } else {
        Serial.println(F("Error sending wakeup POST"));
        Serial.println(http.errorToString(rc));
    }
    yield();
    http.end();
    yield();
    return rc;
}

int Tesla::authorize(const char* password) {
    Serial.print(F("Re-authorization mit password >>"));
    Serial.print(password);
    Serial.println(F("<<"));

    HTTPClient http;

    beginRequest(&http, _auth_url);
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
    Serial.println(reqData);
    
    int rc = http.POST(reqData);
    Serial.print(F("Trying to renew bearer token: "));
    Serial.println(rc);
    yield();
 
    if(rc>0) {
      
        Serial.println(_auth_url);
        String response = http.getString();    
        Serial.println(response);  
        wc.sendClients(response.c_str());
        DynamicJsonDocument doc(256); 
        deserializeJson(doc, response);
        String rt = doc["refresh_token"];
        _authorization = new char[rt.length()+1];
        rt.toCharArray(_authorization, rt.length()+1); 
               
        yield();
        config.save();
        Serial.println(response);  
        Serial.print(F("Authorization: "));
        Serial.println(_authorization); 
        Serial.print(F("VehicleId: "));
        Serial.println(_vehicle_id);         
        Serial.println(F("Der AuthToken wurde erneuert und gespeichert"));       
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
        Serial.println(F("Error sending wakeup POST"));
        Serial.println(http.errorToString(rc));
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
  Serial.print(F("Trying to issue read charge state: "));
  Serial.println(rc);
  yield();
  
  if(rc>0) {
      Serial.println(_get_charge_state_url);
      String response = http.getString();    
      Serial.println(response);  
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
      Serial.print(F("Error sending GET charge state; rc="));
      Serial.println(rc);
      Serial.println(http.errorToString(rc));
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
    Serial.print(F("Trying to issue charge start: "));
    Serial.println(rc);
    
    if(rc>0) {  
        if(debugTesla) {
          Serial.println(_charge_start_url);
          String response = http.getString();                        
          Serial.println(response);  
          wc.sendClients(response.c_str());
        }
    } else {
        Serial.println(F("Error sending POST charge start"));
        Serial.println(http.errorToString(rc));
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
    Serial.print(F("Trying to issue charge stop: "));
    Serial.println(rc);
    yield();

    if(rc>0) {  
        Serial.println(_charge_stop_url);    
        String response = http.getString(); 
        Serial.println(response); 
        wc.sendClients(response.c_str());
    } else {
        Serial.println(F("Error sending POST charge stop"));
        Serial.println(http.errorToString(rc));
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
  Serial.println(reqData);
  yield();

  int rc = http.POST(reqData);
  yield();
  Serial.print(F("Trying to issue set charge limit: "));
  Serial.println(rc);
  
  if(rc>0) {  
      if(debugTesla) {
        Serial.println(_set_charge_limit_url);
        String response = http.getString();                        
        Serial.println(response);  
        wc.sendClients(response.c_str());
      }
  } else {
      Serial.println(F("Error sending POST charge limit"));
      Serial.println(http.errorToString(rc));
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
  Serial.println(F("--------------------------------"));
  Serial.println(_set_charge_limit_url);
  Serial.println(_charge_start_url);
  Serial.println(_charge_stop_url);
  Serial.println(_get_charge_state_url);
  Serial.println(_wakeup_url);
  Serial.println(_auth_url);
  Serial.print(F("Has update: "));
  Serial.println(_hasUpdate);
  Serial.print(F("Chargerate: "));
  Serial.println(_chargeRate);
  Serial.print(F("ChargePhases: "));
  Serial.println(_chargerPhases);
  Serial.print(F("ChargerActualCurrent: "));
  Serial.println(_chargerActualCurrent);
  Serial.print(F("ChargerPower: "));
  Serial.println(_chargerPower);
  Serial.print(F("BatteryLevel: "));
  Serial.println(_batteryLevel);
  Serial.print(F("ChargeLimitSoc: "));
  Serial.println(_chargeLimitSoc);
  Serial.print(F("ChargingState: "));
  Serial.println(_chargingState);
  Serial.print(F("Ladespannung: "));
  Serial.println(_chargerVoltage);  
  Serial.print(F("VehicleId: "));
  Serial.println(_vehicle_id);   
  Serial.print(F("Bearer Token: "));
  Serial.println(_authorization);  
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
      Serial.print(F("Is connected: "));
      Serial.println(client->connected());      
      Serial.print(F("Setting Header: "));
      Serial.print(_hd_user_agent);
      Serial.print(F(" to "));
      Serial.println(_user_agent);
      Serial.print(F("Setting Header: "));
      Serial.print(_hd_app_agent);
      Serial.print(F(" to "));
      Serial.println(_tesla_user_agent);      
      Serial.print(F("Setting Header: "));
      Serial.print(_hd_content_type);
      Serial.print(F(" to "));
      Serial.println(_json_header);            
    }
    client->addHeader(_hd_user_agent, _user_agent);
    client->addHeader(_hd_app_agent, _tesla_user_agent);
    client->addHeader(_hd_content_type, _json_header);
    String bearerToken = String((char*)0);
    bearerToken.reserve(96);
    bearerToken+=F("Bearer ");
    bearerToken+=_authorization;
    client->addHeader(_hd_authorization, bearerToken);   
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
