/*
 * -------------------------------------------------------------------
 * EmonESP Serial to Emoncms gateway
 * -------------------------------------------------------------------
 * Adaptation of Chris Howells OpenEVSE ESP Wifi
 * by Trystan Lea, Glyn Hudson, OpenEnergyMonitor
 * All adaptation GNU General Public License as below.
 *
 * -------------------------------------------------------------------
 *
 * This file is part of OpenEnergyMonitor.org project.
 * EmonESP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * EmonESP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with EmonESP; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "emonesp.h"
#include "config.h"
#include "wifi.h"
#include "web_server.h"
#include "ota.h"
#include "emoncms.h"
#include "mqtt.h"
#include "measure.h"
#include "relay.h"
#include "RTCDS1307.h"
#include "SDcard.h"

const long interval = 5000;
unsigned long previousMillis = 0;
const long interval_readTime = 1000;
unsigned long previousMillis_readTime = 0;
const long interval_logger = 30000;
unsigned long previousMillis_logger = 0;
String writedata="";

void setup() {
  Serial.begin(115200);
#ifdef DEBUG_SERIAL1
  Serial1.begin(115200);
#endif
  pinMode(sensorPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S0, OUTPUT);
  DEBUG.println();
  DEBUG.print("i-emc ");
  DEBUG.println(ESP.getChipId());
  DEBUG.println("Firmware: "+ currentfirmware);
  config_load_settings();
  setup_rtc();
  //config_save_rtc("1");
  if(setrtc!="1"){
    setup_date_on_rtc();
  }
  SD_setup();
  wifi_setup();
  web_server_setup();
  //ota_setup();
  if(laststateload=="1"){
    TurnONLoad();
  }
  else{
    TurnOFFLoad();
  }
  if(sch_state=="1"){
    scheduleTime();
  }
  DEBUG.println("Server started");
  delay(100);
} // end setup

void loop()
{
  if (millis() - previousMillis_readTime >= interval_readTime){
    readtime();previousMillis_readTime=millis();
  }
  //ota_loop();
  web_server_loop();
  wifi_loop();
  //String input = "";
  //boolean gotInput = input_get(input);
  if (millis() - previousMillis >= interval){
    measure();previousMillis=millis();
    Serial.print("Heap : ");Serial.println(String(ESP.getFreeHeap()));
  }
  if (wifi_mode == WIFI_MODE_STA || wifi_mode == WIFI_MODE_AP_AND_STA)
  {
    //if(emoncms_apikey != 0 && gotInput) {
      //emoncms_publish(input);
    //}
    if(mqtt_server != 0)
    {
      mqtt_loop();
    }
  }
  if (SDcard_State=="1") {
    if (millis() - previousMillis_logger >= interval_logger) {
      writedata = RTCTime+"," + String(_Irms)+","+String(_Vrms)+","+String(correctedP);
      SD_write("database.txt",writedata);previousMillis_logger=millis();
    }
  }
} // end loop
