#include "emonesp.h"
#include "mqtt.h"
#include "config.h"
#include <Arduino.h>
#include <PubSubClient.h>             // MQTT https://github.com/knolleary/pubsubclient PlatformIO lib: 89
#include <WiFiClient.h>
#include "relay.h"
#include "wifi.h"

/*// Possible values for client.state()
#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5*/

WiFiClient espClient;                 // Create client for MQTT
PubSubClient mqttclient(espClient);   // Create client for MQTT

long lastMqttReconnectAttempt = 0;
int clientTimeout = 0;
int i = 0;
String connectbroker;
String ip;
String mqtt_pub_irms;
String mqtt_pub_vrms;
String mqtt_pub_realpower;
String mqtt_pub_apparentpower;
String mqtt_pub_powerfactor;
String mqtt_pub_loadstate;
String mqtt_sub_usersetload;
String mqtt_sub_sch;
String mqtt_sub_sch_setstate;
String mqtt_sub_sch_setdatestart;
String mqtt_sub_sch_setdatestop;
String mqtt_sub_sch_settimestart;
String mqtt_sub_sch_settimestop;
String mqtt_pub_sch_state;
String mqtt_pub_sch_datestart;
String mqtt_pub_sch_datestop;
String mqtt_pub_sch_timestart;
String mqtt_pub_sch_timestop;
long timeMQTT=0;
long timeMQTT_stop=0;
int count_mqttdisconnect=0;
// -------------------------------------------------------------------
// MQTT Connect
// -------------------------------------------------------------------
void mqtt_setup()
{
  connectbroker=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/connectbroker";
  ip=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/ip";
  mqtt_pub_irms=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/irms";
  mqtt_pub_vrms=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/vrms";
  mqtt_pub_realpower=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/realpower";
  mqtt_pub_apparentpower=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/apparentpower";
  mqtt_pub_powerfactor=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/powerfactor";
  mqtt_pub_loadstate=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/loadstate";
  mqtt_sub_usersetload=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/setload";
  mqtt_sub_sch=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/#";
  mqtt_sub_sch_setstate=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/setstate";
  mqtt_sub_sch_setdatestart=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/setdatestart";
  mqtt_sub_sch_setdatestop=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/setdatestop";
  mqtt_sub_sch_settimestart=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/settimestart";
  mqtt_sub_sch_settimestop=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/settimestop";
  mqtt_pub_sch_state=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/state";
  mqtt_pub_sch_datestart=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/datestart";
  mqtt_pub_sch_datestop=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/datestop";
  mqtt_pub_sch_timestart=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/timestart";
  mqtt_pub_sch_timestop=mqtt_topic+"/Power/"+String(ESP.getChipId())+"/sch/timestop";
}
// -------------------------------------------------------------------
// MQTT Connect
// -------------------------------------------------------------------
boolean mqtt_connect()
{
  mqttclient.setServer(mqtt_server.c_str(), 1883);
  DEBUG.print("MQTT Connecting...");
  DEBUG.println(mqtt_server);
  String strID = String(ESP.getChipId());
  //boolean connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
  //if (mqttclient.connect(strID.c_str(), mqtt_user.c_str(), mqtt_pass.c_str(), connectbroker.c_str(), 1, true, "0")) {  // Attempt to connect
  if (mqttclient.connect(strID.c_str(), connectbroker.c_str(), 1, true, "0")) {  // Attempt to connect
    DEBUG.println("MQTT connected");
    mqttclient.publish(connectbroker.c_str(), "1", true); // Once connected, publish an announcement..
    //digitalWrite(ledredPin, HIGH);
    mqttclient.publish(ip.c_str(), ipaddress.c_str(), true);
	mqttclient.publish(mqtt_pub_loadstate.c_str(), String(state_load).c_str(), true);

	mqttclient.publish(mqtt_pub_sch_state.c_str(), sch_state.c_str(), true);
	mqttclient.publish(mqtt_pub_sch_datestart.c_str(), sch_datestart.c_str(), true);
	mqttclient.publish(mqtt_pub_sch_datestop.c_str(), sch_datestop.c_str(), true);
	mqttclient.publish(mqtt_pub_sch_timestart.c_str(), sch_timestart.c_str(), true);
	mqttclient.publish(mqtt_pub_sch_timestop.c_str(), sch_timestop.c_str(), true);

    mqttclient.subscribe(mqtt_sub_usersetload.c_str(), 1);
	mqttclient.subscribe(mqtt_sub_sch.c_str(), 1);
    timeMQTT=millis();
  } else {
    DEBUG.print("MQTT failed: ");
    DEBUG.println(mqttclient.state());
    //digitalWrite(ledredPin, LOW);
    timeMQTT=0;
    count_mqttdisconnect++;
    return(0);
  }
  return (1);
}

void mqtt_publish(String topic, String payload)
{
    mqttclient.publish(topic.c_str(), payload.c_str(), true);
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String value=(char*)payload;
  if(String(topic)==mqtt_sub_usersetload){
    if(value=="1"){
      TurnONLoad();
      mqttclient.publish(mqtt_pub_loadstate.c_str(), "1", true);
      Serial.println("load on");
    }
    if(value=="0"){
      TurnOFFLoad();
      mqttclient.publish(mqtt_pub_loadstate.c_str(), "0", true);
      Serial.println("load off");
    }
  }
  else if(String(topic)==mqtt_sub_sch_setstate){
	  config_save_state_scheduler(value);
	  mqttclient.publish(mqtt_pub_sch_state.c_str(), sch_state.c_str(), true);
  }
  else if(String(topic)==mqtt_sub_sch_setdatestart){
	  config_save_scheduler_datestart(value);
	  mqttclient.publish(mqtt_pub_sch_datestart.c_str(), sch_datestart.c_str(), true);
  }
  else if(String(topic)==mqtt_sub_sch_setdatestop){
	  config_save_scheduler_datestop(value);
	  mqttclient.publish(mqtt_pub_sch_datestop.c_str(), sch_datestop.c_str(), true);
  }
  else if(String(topic)==mqtt_sub_sch_settimestart){
	  config_save_scheduler_timestart(value);
	  mqttclient.publish(mqtt_pub_sch_timestart.c_str(), sch_timestart.c_str(), true);
  }
  else if(String(topic)==mqtt_sub_sch_settimestop){
	  config_save_scheduler_timestop(value);
	  mqttclient.publish(mqtt_pub_sch_timestop.c_str(), sch_timestop.c_str(), true);
  }
}

// -------------------------------------------------------------------
// MQTT state management
//
// Call every time around loop() if connected to the WiFi
// -------------------------------------------------------------------
void mqtt_loop()
{
  //digitalWrite(ledredPin, HIGH);
  if (!mqttclient.connected()) {
    long now = millis();
    // try and reconnect continuously for first 5s then try again once every 10s
    if ( (now < 50000) || ((now - lastMqttReconnectAttempt)  > 100000) ) {
      lastMqttReconnectAttempt = now;
      if (mqtt_connect()) { // Attempt to reconnect
        lastMqttReconnectAttempt = 0;
      }
    }
    timeMQTT=0;
    timeMQTT_stop=millis();
    //digitalWrite(ledredPin, HIGH);
  } else {
    timeMQTT=millis()-timeMQTT_stop;
    // if MQTT connected
    mqttclient.loop();
    mqttclient.setCallback(callback);
  }
}

void mqtt_restart()
{
  if (mqttclient.connected()) {
    mqttclient.disconnect();
  }
}

boolean mqtt_connected()
{
  return mqttclient.connected();
}
