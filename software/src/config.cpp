#include "emonesp.h"
#include "config.h"
#include <Arduino.h>
#include <EEPROM.h>                   // Save config settings

String esid = "";
String epass = "";
String setrtc = "";
String www_username = "";
String www_password = "";
String emoncms_server = "";
String emoncms_node = "";
String emoncms_apikey = "";
String emoncms_fingerprint = "";
String mqtt_server = "";
String mqtt_topic = "";
String mqtt_user = "";
String mqtt_pass = "";
String mqtt_feed_prefix = "";
String sch_datestart= "";
String sch_timestart= "";
String sch_datestop= "";
String sch_timestop= "";
String sch_state= "";
String SDcard_State="";
String laststateload="";

#define EEPROM_ESID_SIZE          32
#define EEPROM_EPASS_SIZE         64
#define EEPROM_EMON_API_KEY_SIZE  32
#define EEPROM_EMON_SERVER_SIZE   45
#define EEPROM_EMON_NODE_SIZE     32
#define EEPROM_MQTT_SERVER_SIZE   45
#define EEPROM_MQTT_TOPIC_SIZE    32
#define EEPROM_MQTT_USER_SIZE     32
#define EEPROM_MQTT_PASS_SIZE     64
#define EEPROM_EMON_FINGERPRINT_SIZE  60
#define EEPROM_MQTT_FEED_PREFIX_SIZE  10
#define EEPROM_WWW_USER_SIZE      16
#define EEPROM_WWW_PASS_SIZE      16
#define EEPROM_SETRTC_SIZE      1
#define EEPROM_SCH_DATESTART_SIZE 12
#define EEPROM_SCH_TIMESTART_SIZE 12
#define EEPROM_SCH_DATESTOP_SIZE 12
#define EEPROM_SCH_TIMESTOP_SIZE 12
#define EEPROM_SCH_STATE_SIZE 1
#define EEPROM_SDCARD_STATE_SIZE 1
#define EEPROM_LASTSTATELOAD_SIZE 1
#define EEPROM_SIZE 4096

#define EEPROM_ESID_START         0
#define EEPROM_ESID_END           (EEPROM_ESID_START + EEPROM_ESID_SIZE)
#define EEPROM_EPASS_START        EEPROM_ESID_END
#define EEPROM_EPASS_END          (EEPROM_EPASS_START + EEPROM_EPASS_SIZE)
#define EEPROM_EMON_API_KEY_START EEPROM_EPASS_END
#define EEPROM_EMON_API_KEY_END   (EEPROM_EMON_API_KEY_START + EEPROM_EMON_API_KEY_SIZE)
#define EEPROM_EMON_SERVER_START  EEPROM_EMON_API_KEY_END
#define EEPROM_EMON_SERVER_END    (EEPROM_EMON_SERVER_START + EEPROM_EMON_SERVER_SIZE)
#define EEPROM_EMON_NODE_START    EEPROM_EMON_SERVER_END
#define EEPROM_EMON_NODE_END      (EEPROM_EMON_NODE_START + EEPROM_EMON_NODE_SIZE)
#define EEPROM_MQTT_SERVER_START  EEPROM_EMON_NODE_END
#define EEPROM_MQTT_SERVER_END    (EEPROM_MQTT_SERVER_START + EEPROM_MQTT_SERVER_SIZE)
#define EEPROM_MQTT_TOPIC_START   EEPROM_MQTT_SERVER_END
#define EEPROM_MQTT_TOPIC_END     (EEPROM_MQTT_TOPIC_START + EEPROM_MQTT_TOPIC_SIZE)
#define EEPROM_MQTT_USER_START    EEPROM_MQTT_TOPIC_END
#define EEPROM_MQTT_USER_END      (EEPROM_MQTT_USER_START + EEPROM_MQTT_USER_SIZE)
#define EEPROM_MQTT_PASS_START    EEPROM_MQTT_USER_END
#define EEPROM_MQTT_PASS_END      (EEPROM_MQTT_PASS_START + EEPROM_MQTT_PASS_SIZE)
#define EEPROM_EMON_FINGERPRINT_START  EEPROM_MQTT_PASS_END
#define EEPROM_EMON_FINGERPRINT_END    (EEPROM_EMON_FINGERPRINT_START + EEPROM_EMON_FINGERPRINT_SIZE)
#define EEPROM_MQTT_FEED_PREFIX_START  EEPROM_EMON_FINGERPRINT_END
#define EEPROM_MQTT_FEED_PREFIX_END    (EEPROM_MQTT_FEED_PREFIX_START + EEPROM_MQTT_FEED_PREFIX_SIZE)
#define EEPROM_WWW_USER_START     EEPROM_MQTT_FEED_PREFIX_END
#define EEPROM_WWW_USER_END       (EEPROM_WWW_USER_START + EEPROM_WWW_USER_SIZE)
#define EEPROM_WWW_PASS_START     EEPROM_WWW_USER_END
#define EEPROM_WWW_PASS_END       (EEPROM_WWW_PASS_START + EEPROM_WWW_PASS_SIZE)
#define EEPROM_SETRTC_START       EEPROM_WWW_PASS_END
#define EEPROM_SETRTC_END         (EEPROM_SETRTC_START+EEPROM_SETRTC_SIZE)
#define EEPROM_SCH_DATESTART_START		EEPROM_SETRTC_END
#define EEPROM_SCH_DATESTART_END		(EEPROM_SCH_DATESTART_START+EEPROM_SCH_DATESTART_SIZE)
#define EEPROM_SCH_TIMESTART_START		EEPROM_SCH_DATESTART_END
#define EEPROM_SCH_TIMESTART_END		(EEPROM_SCH_TIMESTART_START+EEPROM_SCH_TIMESTART_SIZE)
#define EEPROM_SCH_DATESTOP_START		EEPROM_SCH_TIMESTART_END
#define EEPROM_SCH_DATESTOP_END			(EEPROM_SCH_DATESTOP_START+EEPROM_SCH_DATESTOP_SIZE)
#define EEPROM_SCH_TIMESTOP_START		EEPROM_SCH_DATESTOP_END
#define EEPROM_SCH_TIMESTOP_END			(EEPROM_SCH_TIMESTOP_START+EEPROM_SCH_TIMESTOP_SIZE)
#define EEPROM_SCH_STATE_START			EEPROM_SCH_TIMESTOP_END
#define EEPROM_SCH_STATE_END			(EEPROM_SCH_STATE_START+EEPROM_SCH_STATE_SIZE)
#define EEPROM_SDCARD_STATE_START		EEPROM_SCH_STATE_END
#define EEPROM_SDCARD_STATE_END			(EEPROM_SDCARD_STATE_START+EEPROM_SDCARD_STATE_SIZE)
#define EEPROM_LASTSTATELOAD_START		EEPROM_SDCARD_STATE_END
#define EEPROM_LASTSTATELOAD_END			(EEPROM_LASTSTATELOAD_START+EEPROM_LASTSTATELOAD_SIZE)

void ResetEEPROM(){
  for (int i = 0; i < EEPROM_SIZE; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void EEPROM_read_string(int start, int count, String& val) {
  for (int i = 0; i < count; ++i){
    byte c = EEPROM.read(start+i);
    if (c!=0 && c!=255) val += (char) c;
  }
}

void EEPROM_write_string(int start, int count, String val) {
  for (int i = 0; i < count; ++i){
    if (i<val.length()) {
      EEPROM.write(start+i, val[i]);
    } else {
      EEPROM.write(start+i, 0);
    }
  }
}

void config_load_settings()
{
  EEPROM.begin(EEPROM_SIZE);
  //config_reset();
  EEPROM_read_string(EEPROM_ESID_START, EEPROM_ESID_SIZE, esid);
  EEPROM_read_string(EEPROM_EPASS_START, EEPROM_EPASS_SIZE, epass);
  EEPROM_read_string(EEPROM_SETRTC_START, EEPROM_SETRTC_SIZE, setrtc);
  EEPROM_read_string(EEPROM_SCH_DATESTART_START, EEPROM_SCH_DATESTART_SIZE, sch_datestart);
  EEPROM_read_string(EEPROM_SCH_TIMESTART_START, EEPROM_SCH_TIMESTART_SIZE, sch_timestart);
  EEPROM_read_string(EEPROM_SCH_DATESTOP_START, EEPROM_SCH_DATESTOP_SIZE, sch_datestop);
  EEPROM_read_string(EEPROM_SCH_TIMESTOP_START, EEPROM_SCH_TIMESTOP_SIZE, sch_timestop);
  EEPROM_read_string(EEPROM_SCH_STATE_START, EEPROM_SCH_STATE_SIZE, sch_state);
  if(sch_state==""){sch_state="0";}
  EEPROM_read_string(EEPROM_SDCARD_STATE_START, EEPROM_SDCARD_STATE_SIZE, SDcard_State);
  if(SDcard_State==""){SDcard_State="0";}
  EEPROM_read_string(EEPROM_LASTSTATELOAD_START, EEPROM_LASTSTATELOAD_SIZE, laststateload);
  EEPROM_read_string(EEPROM_EMON_API_KEY_START, EEPROM_EMON_API_KEY_SIZE, emoncms_apikey);
  EEPROM_read_string(EEPROM_EMON_SERVER_START, EEPROM_EMON_SERVER_SIZE, emoncms_server);
  EEPROM_read_string(EEPROM_EMON_NODE_START, EEPROM_EMON_NODE_SIZE, emoncms_node);
  EEPROM_read_string(EEPROM_EMON_FINGERPRINT_START, EEPROM_EMON_FINGERPRINT_SIZE, emoncms_fingerprint);
  EEPROM_read_string(EEPROM_MQTT_SERVER_START, EEPROM_MQTT_SERVER_SIZE, mqtt_server);
  EEPROM_read_string(EEPROM_MQTT_TOPIC_START, EEPROM_MQTT_TOPIC_SIZE, mqtt_topic);
  EEPROM_read_string(EEPROM_MQTT_FEED_PREFIX_START, EEPROM_MQTT_FEED_PREFIX_SIZE, mqtt_feed_prefix);
  EEPROM_read_string(EEPROM_MQTT_USER_START, EEPROM_MQTT_USER_SIZE, mqtt_user);
  EEPROM_read_string(EEPROM_MQTT_PASS_START, EEPROM_MQTT_PASS_SIZE, mqtt_pass);
  EEPROM_read_string(EEPROM_WWW_USER_START, EEPROM_WWW_USER_SIZE, www_username);
  EEPROM_read_string(EEPROM_WWW_PASS_START, EEPROM_WWW_PASS_SIZE, www_password);
}
void config_save_scheduler(String datestart, String timestart, String datestop, String timestop){
  sch_datestart = datestart;
  sch_timestart = timestart;
  sch_datestop = datestop;
  sch_timestop = timestop;
  EEPROM_write_string(EEPROM_SCH_DATESTART_START, EEPROM_SCH_DATESTART_SIZE, sch_datestart);
  EEPROM_write_string(EEPROM_SCH_TIMESTART_START, EEPROM_SCH_TIMESTART_SIZE, sch_timestart);
  EEPROM_write_string(EEPROM_SCH_DATESTOP_START, EEPROM_SCH_DATESTOP_SIZE, sch_datestop);
  EEPROM_write_string(EEPROM_SCH_TIMESTOP_START, EEPROM_SCH_TIMESTOP_SIZE, sch_timestop);
  EEPROM.commit();
}

void config_save_scheduler_datestart(String datestart){
  sch_datestart = datestart;
  EEPROM_write_string(EEPROM_SCH_DATESTART_START, EEPROM_SCH_DATESTART_SIZE, sch_datestart);
  EEPROM.commit();
}

void config_save_scheduler_timestart(String timestart){
  sch_timestart = timestart;
  EEPROM_write_string(EEPROM_SCH_TIMESTART_START, EEPROM_SCH_TIMESTART_SIZE, sch_timestart);
  EEPROM.commit();
}

void config_save_scheduler_datestop(String datestop){
  sch_datestop = datestop;
  EEPROM_write_string(EEPROM_SCH_DATESTOP_START, EEPROM_SCH_DATESTOP_SIZE, sch_datestop);
  EEPROM.commit();
}

void config_save_scheduler_timestop(String timestop){
  sch_timestop = timestop;
  EEPROM_write_string(EEPROM_SCH_TIMESTOP_START, EEPROM_SCH_TIMESTOP_SIZE, sch_timestop);
  EEPROM.commit();
}

void config_save_state_scheduler(String state){
  sch_state = state;
  EEPROM_write_string(EEPROM_SCH_STATE_START, EEPROM_SCH_STATE_SIZE, sch_state);
  EEPROM.commit();
}

void config_save_state_SDcard(String state){
  SDcard_State = state;
  EEPROM_write_string(EEPROM_SDCARD_STATE_START, EEPROM_SDCARD_STATE_SIZE, SDcard_State);
  EEPROM.commit();
}

void config_save_state_load(String state){
  EEPROM_write_string(EEPROM_LASTSTATELOAD_START, EEPROM_LASTSTATELOAD_SIZE, state);
  EEPROM.commit();
}

void config_save_emoncms(String server, String node, String apikey, String fingerprint)
{
  emoncms_server = server;
  emoncms_node = node;
  emoncms_apikey = apikey;
  emoncms_fingerprint = fingerprint;
  EEPROM_write_string(EEPROM_EMON_API_KEY_START, EEPROM_EMON_API_KEY_SIZE, emoncms_apikey);
  EEPROM_write_string(EEPROM_EMON_SERVER_START, EEPROM_EMON_SERVER_SIZE, emoncms_server);
  EEPROM_write_string(EEPROM_EMON_NODE_START, EEPROM_EMON_NODE_SIZE, emoncms_node);
  EEPROM_write_string(EEPROM_EMON_FINGERPRINT_START, EEPROM_EMON_FINGERPRINT_SIZE, emoncms_fingerprint);
  EEPROM.commit();
}

void config_save_rtc(String _setrtc)
{
  setrtc = _setrtc;
  EEPROM_write_string(EEPROM_SETRTC_START, EEPROM_SETRTC_SIZE, setrtc);
  EEPROM.commit();
}

void config_save_mqtt(String server, String topic, String prefix, String user, String pass)
{
  mqtt_server = server;
  mqtt_topic = topic;
  mqtt_feed_prefix = prefix;
  mqtt_user = user;
  mqtt_pass = pass;
  EEPROM_write_string(EEPROM_MQTT_SERVER_START, EEPROM_MQTT_SERVER_SIZE, mqtt_server);
  EEPROM_write_string(EEPROM_MQTT_TOPIC_START, EEPROM_MQTT_TOPIC_SIZE, mqtt_topic);
  EEPROM_write_string(EEPROM_MQTT_FEED_PREFIX_START, EEPROM_MQTT_FEED_PREFIX_SIZE, mqtt_feed_prefix);
  EEPROM_write_string(EEPROM_MQTT_USER_START, EEPROM_MQTT_USER_SIZE, mqtt_user);
  EEPROM_write_string(EEPROM_MQTT_PASS_START, EEPROM_MQTT_PASS_SIZE, mqtt_pass);
  EEPROM.commit();
}

void config_save_admin(String user, String pass)
{
  www_username = user;
  www_password = pass;
  EEPROM_write_string(EEPROM_WWW_USER_START, EEPROM_WWW_USER_SIZE, user);
  EEPROM_write_string(EEPROM_WWW_PASS_START, EEPROM_WWW_PASS_SIZE, pass);
  EEPROM.commit();
}

void config_save_wifi(String qsid, String qpass)
{
  esid = qsid;
  epass = qpass;
  EEPROM_write_string(EEPROM_ESID_START, EEPROM_ESID_SIZE, qsid);
  EEPROM_write_string(EEPROM_EPASS_START, EEPROM_EPASS_SIZE, qpass);
  EEPROM.commit();
}

void config_reset_savewifi()
{
  esid ="";
  epass ="";
  EEPROM_write_string(EEPROM_ESID_START, EEPROM_ESID_SIZE, esid);
  EEPROM_write_string(EEPROM_EPASS_START, EEPROM_EPASS_SIZE, epass);
  EEPROM.commit();
}

void config_reset(){ResetEEPROM();}
