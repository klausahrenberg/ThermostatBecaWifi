#ifndef __KA_NETWORK_H
#define __KA_NETWORK_H

#include "Arduino.h"
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include "KaPubSubClient.h"
#include <ArduinoJson.h>

const static char HTTP_HEAD[]               PROGMEM = {"<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>"};
const static char HTTP_STYLE[]              PROGMEM = {"<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family: Open Sans;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>"};
const static char HTTP_SCRIPT[]             PROGMEM = {"<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>"};
const static char HTTP_HEAD_END[]           PROGMEM = {"</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>"};
const static char HTTP_BUTTON_CONFIGURE[]   PROGMEM = {"<form action=\"/wifi\" method=\"get\"><button>Configure device</button></form><br>"};
const static char HTTP_BUTTON_FIRMWARE[]    PROGMEM = {"<form action=\"/firmware\" method=\"get\"><button>Update firmware</button></form><br>"};
const static char HTTP_BUTTON_INFO[]        PROGMEM = {"<form action=\"/info\" method=\"get\"><button>Info</button></form><br>"};
const static char HTTP_BUTTON_RESET[]       PROGMEM = {"<form action=\"/reset\" method=\"post\"><button>Reset</button></form>"};
const static char HTTP_FORM_WIFI_PARAM[]    PROGMEM = {"<form method='get' action='saveConfiguration'>SSID:<br><input id='s' name='s' length=32 placeholder='WLAN name (only 2G)' value='{s}'><br/>Wifi Password:<br><input id='p' name='p' length=64 type='password' placeholder='' value='{p}'><br/>"};
const static char HTTP_FORM_MQTT_PARAM[]    PROGMEM = {"<br>MQTT Server:<br><input id='ms' name='ms' length=32 placeholder='node' value='{ms}'><br/><br>MQTT Port:<br><input id='mpo' name='mpo' length=32 placeholder='1883' value='{mpo}'><br/>Topic:<br><input id='mt' name='mt' length=64 placeholder='home/room/thing' value='{mt}'><br/>MQTT user:<br><input id='mu' name='mu' length=32 placeholder='' value='{mu}'><br/>MQTT password:<br><input id='mp' name='mp' length=64 type='password' placeholder=''  value='{mp}'><br/>"};
const static char HTTP_FORM_END[]           PROGMEM = {"<br/><button type='submit'>Save configuration</button></form>"};
const static char HTTP_SAVED[]              PROGMEM = {"<div>{v}<br/>Try reboot ESP.<br />If connect fails, start configuration again.</div><br><form action=\"/\" method=\"get\"><button>Back to configuration</button></form>"};
const static char HTTP_END[]                PROGMEM = {"</div></body></html>"};
const static char HTTP_FORM_FIRMWARE[]      PROGMEM = {"<form method='POST' action='' enctype='multipart/form-data'> <input type='file' accept='.bin' name='update'><br><br> <button type='submit'>Update firmware</button> </form>"};

//<input type='file' name='update'>
//<input type='submit' value='Update firmware'>

class KaNetwork {
public:
	typedef std::function<void(void)> THandlerFunction;
	typedef std::function<void(String topic, String payload)> TCallBackMqttHandler;
	KaNetwork(String applicationName, String firmwareVersion, bool debug, bool startConfigIfNoSettingsFound);
	virtual ~KaNetwork();
	bool loadSettings();
	void saveSettings();
	bool loop(bool waitForWifiConnection);
	void setOnNotify(THandlerFunction onNotify);
	void setOnConfigurationFinished(THandlerFunction  onConfigurationFinished);
	bool publishMqtt(String topic, JsonObject& json);
	bool publishMqtt(String topic, String payload);
	void onCallbackMqtt(TCallBackMqttHandler cbmh);
	void startWebServer();
	void stopWebServer();
	void enableWebServer(bool startWebServer);
	bool isWebServerRunning();
	bool isUpdateRunning();
	bool isSoftAP();
	bool isWifiConnected();
	bool isMqttConnected();
	String getDeviceIpAddress();
private:
	THandlerFunction onNotify;
	THandlerFunction onConfigurationFinished;
	TCallBackMqttHandler _mqtt_callback;
	bool debug, settingsStored, updateRunning;
	DNSServer *dnsApServer;
	ESP8266WebServer *webServer;
	void log(String debugMessage);
	int networkState;
	String applicationName, firmwareVersion;
	String ssid, password, mqttTopic;
	String mqttServer;
	String mqttPort;
	String mqttUser;
	String mqttPassword;
	String firmwareUpdateError;
	KaPubSubClient *mqttClient;
	long lastMqttConnect, lastWifiConnect;
	void mqttCallback(char* ptopic, byte* payload, unsigned int length);bool mqttReconnect();
	void notify();
	String getHttpCaption();
	String getClientName();
	void handleHttpRootRequest();
	void handleHttpWifi();
	void handleHttpSaveConfiguration();
	void handleHttpInfo();
	void handleHttpReset();
	void handleHttpFirmwareUpdate();
	void handleHttpFirmwareUpdateFinished();
	void handleHttpFirmwareUpdateUpload();
	String getFirmwareUpdateErrorMessage();
	void setFirmwareUpdateError();
	void restart();
};

#endif
