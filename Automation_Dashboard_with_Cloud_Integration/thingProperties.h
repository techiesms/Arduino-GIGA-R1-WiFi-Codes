// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[]     = "SSID";    // Network SSID (name)
const char PASS[]     = "PASSWORD";    // Network password (use for WPA, or use as key for WEP)

void onRelay1Change();
void onRelay2Change();
void onRelay3Change();
void onRelay4Change();
void onTemperatureChange();
void onHumidityChange();
void onFanSpeedChange();

CloudLight relay1;
CloudLight relay2;
CloudLight relay3;
CloudLight relay4;
CloudTemperatureSensor temperature;
CloudRelativeHumidity humidity;
CloudDimmedLight fanSpeed;


void initProperties() {

  ArduinoCloud.addProperty(relay1, READWRITE, ON_CHANGE, onRelay1Change);
  ArduinoCloud.addProperty(relay2, READWRITE, ON_CHANGE, onRelay2Change);
  ArduinoCloud.addProperty(relay3, READWRITE, ON_CHANGE, onRelay3Change);
  ArduinoCloud.addProperty(relay4, READWRITE, ON_CHANGE, onRelay4Change);
  ArduinoCloud.addProperty(temperature, READWRITE, ON_CHANGE, onTemperatureChange);
  ArduinoCloud.addProperty(humidity, READWRITE, ON_CHANGE, onHumidityChange);
  ArduinoCloud.addProperty(fanSpeed, READWRITE, ON_CHANGE, onFanSpeedChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);