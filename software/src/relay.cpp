#include "relay.h"
#include "config.h"

const int relayPin=16;//D0
bool state_load;

void ReadLoadState(){
  state_load=digitalRead(relayPin);
}
void TurnONLoad(){
  digitalWrite(relayPin, HIGH);//turn on load
  ReadLoadState();
  config_save_state_load(String(state_load));
}

void TurnOFFLoad(){
	digitalWrite(relayPin, LOW);//turn on load
  ReadLoadState();
  config_save_state_load(String(state_load));
}
