#include "Tesla.h"

#include "global.h"

//DOKU
//https://tesla-api.timdorr.com/api-basics/authentication

int Tesla::wakeup() {

    HTTPClient http;

    beginRequest(&http, _wakup_url);
    
    int rc = http.POST("");
    Serial.print(F("Trying to issue wakeup: "));
    Serial.println(rc);
 
    if(rc>0) {
        if(debug) {
          Serial.println(_wakup_url);
          String response = http.getString();    
          Serial.println(response);  
        }
        //200 == OK,
        //401 == Bearer Token abgelaufen oder nicht richtig

    } else {
        Serial.println(F("Error sending wakeup POST"));
    }
  
    http.end();
  
    return rc;
}

int Tesla::readChargeState() {

  HTTPClient http;

  beginRequest(&http, _get_charge_state_url);
    
  int rc = http.GET();
  Serial.print(F("Trying to issue read charge state: "));
  Serial.println(rc);
  
  if(rc>0) {
      if(debug) {
        Serial.println(_get_charge_state_url);
        String response = http.getString();    
        Serial.println(response);  
      }
      //200 == OK,
      //401 == Bearer Token abgelaufen oder nicht richtig
      //408 == 'vehicle not available' => Wakup
  } else {
      Serial.println(F("Error sending GET charge state"));
  }

  http.end();
  return rc;
}
    
int Tesla::startCharge() {
  
    HTTPClient http;
    
    beginRequest(&http, _set_charge_limit_url);
    
    int rc = http.POST("{\"percent\":90}");
    Serial.print(F("Trying to issue charge start (through setting charge limit to 90%): "));
    Serial.println(rc);
      
    if(rc>0) {   
        if(debug) {       
          Serial.println(_set_charge_limit_url);
          String response = http.getString(); 
          Serial.println(response);  
        }
        //200 == OK,
        //401 == Bearer Token abgelaufen oder nicht richtig
        //408 == Vehicle not available, do wakeup
        if(rc==200) {

              //FIXME wird der Ladestart direkt nach dem Setzen des neuen Chargelimits versucht,
              //      misslingt der Zugriff (reason:complete). Erst einige Sekunden später geht das.
              delay(5000);
              beginRequest(&http, _charge_start_url);    
              
              int rc = http.POST("");
              Serial.print(F("Trying to issue charge start: "));
              Serial.println(rc);

              if(rc>0) {  
                  if(debug) {
                    Serial.println(_charge_start_url);
                    String response = http.getString();                        
                    Serial.println(response);  
                  }
              } else {
                  Serial.println(F("Error sending POST charge start"));
              }
        }
    } else {
        Serial.println(F("Error sending POST charge start (set_charge_limit to 90%)"));
    }
  
    http.end();
    return rc;   
}

int Tesla::stopCharge() {

    HTTPClient http;

    beginRequest(&http, _charge_stop_url);
    
    int rc = http.POST("");
    Serial.print(F("Trying to issue charge stop: "));
    Serial.println(rc);

    if(rc>0) {  
        if(debug) {    
          Serial.println(_charge_stop_url);    
          String response = http.getString(); 
          Serial.println(response);  
        }
        
        //200 == OK,
        //401 == Bearer Token abgelaufen oder nicht richtig
        //408 == Vehicle not available, do wakeup
        if(rc==200) {
          
                beginRequest(&http, _set_charge_limit_url);
                
                int rc = http.POST("{\"percent\":50}");
                Serial.print(F("Trying to issue set charge limit to 50%: "));
                Serial.println(rc);
                
                if(rc>0) {  
                    if(debug) {
                      Serial.println(_set_charge_limit_url);
                      String response = http.getString();                        
                      Serial.println(response);  
                    }
                } else {
                    Serial.println(F("Error sending POST charge limit 50%"));
                }
        }
    } else {
        Serial.println(F("Error sending POST charge stop"));
    }
  
    http.end();
    return rc;    
}

void Tesla::init(const char* auth, const char* vehicleid) {
  _authorization = new char[strlen(auth)+1];
  strcpy(_authorization, auth);
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
  _wakup_url = new char[wakeupUrl.length()+1];
  strcpy(_wakup_url, wakeupUrl.c_str());
}

void Tesla::print() {
  Serial.println(_set_charge_limit_url);
  Serial.println(_charge_start_url);
  Serial.println(_charge_stop_url);
  Serial.println(_get_charge_state_url);
  Serial.println(_wakup_url);
}

void Tesla::beginRequest(HTTPClient *client, char *url) {
    client->begin(url);
    client->addHeader(_hd_user_agent, _user_agent);
    client->addHeader(_hd_app_agent, _tesla_user_agent);
    client->addHeader(_hd_content_type, _json_header);
    client->addHeader(_hd_authorization, _authorization);   
}

/*
wakeup 200:
{
  "response": {
    "id": 5.0663175238727e+16,
    "user_id": 679189,
    "vehicle_id": 850056567,
    "vin": "5YJ3E7EB4KF196689",
    "display_name": "Perry",
    "option_codes": "AD15,MDL3,PBSB,RENA,BT37,ID3W,RF3G,S3PB,DRLH,DV2W,W39B,APF0,COUS,BC3B,CH07,PC30,FC3P,FG31,GLFR,HL31,HM31,IL31,LTPB,MR31,FM3B,RS3H,SA3P,STCP,SC04,SU3C,T3CA,TW00,TM00,UT3P,WR00,AU3P,APH3,AF00,ZCST,MI00,CDM0",
    "color": null,
    "tokens": [
      "bc83dc46ad1026c6",
      "1d665cab2c922f4a"
    ],
    "state": "asleep",
    "in_service": false,
    "id_s": "50663175238726715",
    "calendar_enabled": true,
    "api_version": 6,
    "backseat_token": null,
    "backseat_token_updated_at": null
  }
}
readChargeState 408:
{
  "response": null,
  "error": "vehicle unavailable: {:error=>\"vehicle unavailable:\"}",
  "error_description": ""
}
readChargeState 200:
{
  "response": {
    "battery_heater_on": false,
    "battery_level": 95,
    "battery_range": 294.73,
    "charge_current_request": 8,
    "charge_current_request_max": 13,
    "charge_enable_request": true,
    "charge_energy_added": 0,
    "charge_limit_soc": 90,
    "charge_limit_soc_max": 100,
    "charge_limit_soc_min": 50,
    "charge_limit_soc_std": 90,
    "charge_miles_added_ideal": 0,
    "charge_miles_added_rated": 0,
    "charge_port_cold_weather_mode": false,
    "charge_port_door_open": true,
    "charge_port_latch": "Engaged",
    "charge_rate": 0,
    "charge_to_max_range": false,
    "charger_actual_current": 0,
    "charger_phases": null,
    "charger_pilot_current": 13,
    "charger_power": 0,
    "charger_voltage": 2,
    "charging_state": "Complete",
    "conn_charge_cable": "IEC",
    "est_battery_range": 275.64,
    "fast_charger_brand": "<invalid>",
    "fast_charger_present": false,
    "fast_charger_type": "MCSingleWireCAN",
    "ideal_battery_range": 294.73,
    "managed_charging_active": false,
    "managed_charging_start_time": null,
    "managed_charging_user_canceled": false,
    "max_range_charge_counter": 1,
    "not_enough_power_to_heat": null,
    "scheduled_charging_pending": false,
    "scheduled_charging_start_time": null,
    "time_to_full_charge": 0,
    "timestamp": 1561015943654,
    "trip_charging": false,
    "usable_battery_level": 95,
    "user_charge_enable_request": null
  }
}
stopCharge 200:
{"response":{"reason":"","result":true}}
startCharge 200:
{"response":{"reason":"","result":true}}
*/
