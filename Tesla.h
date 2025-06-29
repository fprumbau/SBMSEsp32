#ifndef __TESLA_H
#define __TESLA_H

#include <HTTPClient.h> 

class Tesla {
  private:
    const char* _api_base_url = "https://owner-api.teslamotors.com/api/1/vehicles/";
    const char* _hd_user_agent = "User-Agent";
    const char* _hd_app_agent = "X-Tesla-User-Agent";
    const char* _hd_content_type = "Content-Type";
    const char* _hd_authorization = "Authorization";
    const char* _json_header = "application/json";
    const char* _user_agent = "Mozilla/5.0 (Linux; Android 9.0.0; VS985 4G Build/LRX21Y; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/58.0.3029.83 Mobile Safari/537.36";
    const char* _tesla_user_agent = "TeslaApp/3.4.4-350/fad4a582e/android/9.0.0";
    const char* _app_agent = "TeslaApp/3.4.4-350/fad4a582e/android/9.0.0";
    const char* _charge_limit = "/command/set_charge_limit";    
    const char* _charge_state = "/data_request/charge_state";
    const char* _charge_start = "/command/charge_start";
    const char* _charge_stop = "/command/charge_stop";
    const char* _auth = "/oauth/token";
    const char* _wakeup = "/wake_up";
    
    float _chargeRate;  //10.9
    int _chargerPhases; //1
    int _chargerActualCurrent; //11 (A)
    int _chargerPower; //3 (kW)
    int _batteryLevel; //61 (%)
    int _chargeLimitSoc;
    int _chargerVoltage; //229 (V)
    String _chargingState; //Complete|Charging|Stopped|Disconnected|Starting
    //LITTLEFS
    char* _authorization;
    char* _vehicle_id;
    //init(..)
    char* _auth_url;
    char* _wakeup_url;
    char* _vehicle_base_url;
    char* _charge_start_url;
    char* _charge_stop_url;
    char* _get_charge_state_url;
    char* _set_charge_limit_url;

    //Updated flag
    bool _hasUpdate;

    //beginRequest
    void beginRequest(HTTPClient *client, char *url);
        
  public:
    void vehicleId(const char* vehicleId);
    void authorization(const char* auth);
    int authorize(const char* password);
    int wakeup();
    int startCharge();
    int stopCharge();
    int setChargeLimit(int percent);
    int readChargeState();
    int getSoC();
    void print();
    bool isCharging();
    void setCharging(bool charging);
    bool hasUpdate();
    void reset();
    const char* status();
    char* vehicleId();
    char* authorization();
};

#endif
