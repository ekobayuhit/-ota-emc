#ifndef _EMONESP_CONFIG_H
#define _EMONESP_CONFIG_H

#include <Arduino.h>

// -------------------------------------------------------------------
// Load and save the EmonESP config.
//
// This initial implementation saves the config to the EEPROM area of flash
// -------------------------------------------------------------------

// Global config varables

// Wifi Network Strings
extern String esid;
extern String epass;

extern String setrtc;
// Web server authentication (leave blank for none)
extern String www_username;
extern String www_password;

// EMONCMS SERVER strings
extern String emoncms_server;
extern String emoncms_node;
extern String emoncms_apikey;
extern String emoncms_fingerprint;

// MQTT Settings
extern String mqtt_server;
extern String mqtt_topic;
extern String mqtt_user;
extern String mqtt_pass;
extern String mqtt_feed_prefix;

extern String sch_datestart;
extern String sch_timestart;
extern String sch_datestop;
extern String sch_timestop;
extern String sch_state;

extern String SDcard_State;

extern String laststateload;
extern void config_save_scheduler(String datestart, String timestart, String datestop, String timestop);
extern void config_save_scheduler_datestart(String datestart);
extern void config_save_scheduler_timestart(String timestart);
extern void config_save_scheduler_datestop(String datestop);
extern void config_save_scheduler_timestop(String datestart);
extern void config_save_state_scheduler(String state);
extern void config_save_state_SDcard(String state);
extern void config_save_state_load(String state);
// -------------------------------------------------------------------
// Load saved settings
// -------------------------------------------------------------------
extern void config_load_settings();

extern void config_save_rtc(String _setrtc);
// -------------------------------------------------------------------
// Save the EmonCMS server details
// -------------------------------------------------------------------
extern void config_save_emoncms(String server, String node, String apikey, String fingerprint);

// -------------------------------------------------------------------
// Save the MQTT broker details
// -------------------------------------------------------------------
extern void config_save_mqtt(String server, String topic, String prefix, String user, String pass);

// -------------------------------------------------------------------
// Save the admin/web interface details
// -------------------------------------------------------------------
extern void config_save_admin(String user, String pass);

// -------------------------------------------------------------------
// Save the Wifi details
// -------------------------------------------------------------------
extern void config_save_wifi(String qsid, String qpass);


// -------------------------------------------------------------------
// Reset the config back to defaults
// -------------------------------------------------------------------
extern void config_reset();
//
extern void config_reset_savewifi();
#endif // _EMONESP_CONFIG_H
