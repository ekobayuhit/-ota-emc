#ifndef _EMONESP_MQTT_H
#define _EMONESP_MQTT_H

// -------------------------------------------------------------------
// MQTT support
// -------------------------------------------------------------------

#include <Arduino.h>

extern long timeMQTT;
extern int count_mqttdisconnect;

extern String mqtt_pub_irms;
extern String mqtt_pub_vrms;
extern String mqtt_pub_realpower;
extern String mqtt_pub_apparentpower;
extern String mqtt_pub_powerfactor;
extern String mqtt_pub_loadstate;

extern void mqtt_setup();
// -------------------------------------------------------------------
// Perform the background MQTT operations. Must be called in the main
// loop function
// -------------------------------------------------------------------
extern void mqtt_loop();

// -------------------------------------------------------------------
// Publish values to MQTT
//
// data: a comma seperated list of name:value pairs to send
// -------------------------------------------------------------------
extern void mqtt_publish(String data, String payload);

// -------------------------------------------------------------------
// Restart the MQTT connection
// -------------------------------------------------------------------
extern void mqtt_restart();


// -------------------------------------------------------------------
// Return true if we are connected to an MQTT broker, false if not
// -------------------------------------------------------------------
extern boolean mqtt_connected();

#endif // _EMONESP_MQTT_H
