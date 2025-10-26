/*
    CEC to KenwoodXS Simple Broadcast Listener
    
    Listens to CEC broadcast messages without establishing proper CEC connection.
    Reacts to TV power on/off commands to control Kenwood audio device.
    
    TV Commands (from hdmi-cec-commands.txt):
    - Power On: 0F:84:00:00:00 or 0F:87:00:90:3E
    - Standby:  0F:36
*/

#include "Arduino.h"
#include "CEClient.h"
#include "KenwoodXS.h"

// Pin definitions
#define CEC_INPUT_PIN           5
#define CEC_OUTPUT_PIN          6
#define KENWOOD_CTRL_PIN        3
#define KENWOOD_SDAT_PIN        2

// CEC setup
#define CEC_PHYSICAL_ADDRESS    0x1000


// create a CEC client
CEClient ceclient(CEC_PHYSICAL_ADDRESS, CEC_INPUT_PIN, CEC_OUTPUT_PIN);

// create a KenwoodXS instance
KenwoodXS kenwood(KENWOOD_CTRL_PIN, KENWOOD_SDAT_PIN);

// TV state tracking
bool tvIsOn = false;

void cecMessageReceived(int from, int to, unsigned char* data, int len) {
  Serial.print("CEC Message: ");
  Serial.print(from, HEX);
  Serial.print(" -> ");
  Serial.print(to, HEX);
  Serial.print(" Data: ");
  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Check for TV power commands
  if (from == 0x00) { // TV messages
    if (len >= 1) {
      if (data[0] == 0x84 || data[0] == 0x87) {
        if (!tvIsOn) {
          Serial.println("TV Power ON detected -> Kenwood ON + Start CD");
          kenwood.powerOn();
          delay(500);  // Wait for Kenwood to power up
          kenwood.startCD();
          tvIsOn = true;
        } else {
          Serial.println("TV Power ON detected (already on, ignoring)");
        }
      } else if (data[0] == 0x36) {
        Serial.println("TV Standby detected -> Kenwood OFF");
        kenwood.powerOff();
        tvIsOn = false;
      }
    }
  }
}

void setup() {

  Serial.begin(115200);
  
  Serial.println("=== CEC to KenwoodXS Controller ===");
  Serial.println("Listening for TV power commands from 0x00...");
  Serial.println();

  // initialize KenwoodXS
  kenwood.begin();
  Serial.println("KenwoodXS initialized");

  // initialize the client with the default device type (PLAYBACK)
  ceclient.begin();

  // enable promiscuous mode (print all the incoming messages)
  ceclient.setPromiscuous(true);

  // enable monitor mode (do not transmit)
  ceclient.setMonitorMode(true);
  
  // set the callback function
  ceclient.onReceiveCallback(cecMessageReceived);
  
  Serial.println("CEC listener initialized");
  Serial.print("Initial TV state: ");
  Serial.println(tvIsOn ? "ON" : "OFF");
  Serial.println("Ready - listening for TV commands...");
  Serial.println();
}

void loop() {

    // run the client
    ceclient.run();
}