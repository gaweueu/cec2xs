#include <Arduino.h>
#include <CEClient.h>
#include <KenwoodXS.h>

// Pin definitions
const int CEC_INPUT_PIN = 8;   // CEC input pin
const int CEC_OUTPUT_PIN = 9;  // CEC output pin (optional, can be same as input)
const int KENWOOD_CTRL_PIN = 3;  // Kenwood XS control pin
const int KENWOOD_SDAT_PIN = 2;  // Kenwood XS data pin

// Physical address for CEC device (adjust based on your HDMI connection)
const int PHYSICAL_ADDRESS = 0x2000;

// Create instances
CEClient cec(PHYSICAL_ADDRESS, CEC_INPUT_PIN, CEC_OUTPUT_PIN);
KenwoodXS kenwood(KENWOOD_CTRL_PIN, KENWOOD_SDAT_PIN);

// Mapping from CEC commands to KenwoodXS commands
void handleCECCommand(int source, int dest, unsigned char* buffer, int count) {
    if (count < 1) return;
    
    Serial.print("CEC command received from ");
    Serial.print(source, HEX);
    Serial.print(" to ");
    Serial.print(dest, HEX);
    Serial.print(": ");
    
    for (int i = 0; i < count; i++) {
        Serial.print("0x");
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    // Handle common CEC commands and map to KenwoodXS
    switch (buffer[0]) {
        case 0x44: // User Control Pressed
            if (count > 1) {
                switch (buffer[1]) {
                    case 0x44: // Play
                        Serial.println("CEC Play -> Kenwood Start CD");
                        kenwood.startCD();
                        break;
                    case 0x45: // Stop
                        Serial.println("CEC Stop -> Kenwood Stop CD");
                        kenwood.stopCD();
                        break;
                    case 0x46: // Record
                        Serial.println("CEC Record -> Kenwood Tape A Record");
                        kenwood.sendCommand(TA_RECORD);
                        break;
                    case 0x47: // Rewind
                        Serial.println("CEC Rewind -> Kenwood CD Rewind");
                        kenwood.sendCommand(CD_REWIND);
                        break;
                    case 0x48: // Fast Forward
                        Serial.println("CEC Fast Forward -> Kenwood CD Fast Forward");
                        kenwood.sendCommand(CD_FAST_FORWARD);
                        break;
                    case 0x4B: // Pause Play Function
                        Serial.println("CEC Pause -> Kenwood CD Play/Pause");
                        kenwood.playCD();
                        break;
                    default:
                        Serial.print("Unhandled CEC user control: 0x");
                        Serial.println(buffer[1], HEX);
                        break;
                }
            }
            break;
            
        case 0x82: // Active Source
            Serial.println("CEC Active Source -> Kenwood CD Input");
            kenwood.selectInput(CD);
            break;
            
        case 0x9D: // Inactive Source
            Serial.println("CEC Inactive Source -> Kenwood Power Off");
            kenwood.powerOff();
            break;
            
        case 0x04: // Image View On
        case 0x0D: // Text View On
            Serial.println("CEC View On -> Kenwood Power On + Start CD");
            kenwood.powerOn();
            delay(100); // Small delay between commands
            kenwood.startCD();
            break;
            
        case 0x36: // Standby
            Serial.println("CEC Standby -> Kenwood Power Off");
            kenwood.powerOff();
            break;
            
        default:
            Serial.print("Unhandled CEC opcode: 0x");
            Serial.println(buffer[0], HEX);
            break;
    }
}

void onKenwoodCommandSent(byte command) {
    Serial.print("Kenwood command sent: ");
    Serial.print(command, DEC);
    Serial.print(" (0x");
    Serial.print(command, HEX);
    Serial.println(")");
}

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for serial port to connect
    }
    
    Serial.println("CEC to KenwoodXS Bridge Example");
    Serial.println("This example bridges CEC commands to KenwoodXS commands");
    Serial.println();
    
    // Initialize both libraries
    cec.begin();
    kenwood.begin();
    
    // Set up callbacks
    cec.onReceiveCallback(handleCECCommand);
    kenwood.setDebugOutput(true);
    kenwood.onCommandSentCallback(onKenwoodCommandSent);
    
    // Enable promiscuous mode to receive all CEC messages
    cec.setPromiscuous(true);
    
    Serial.println("Waiting for CEC device to be ready...");
    while (!cec.isReady()) {
        cec.run();
        delay(100);
    }
    
    Serial.print("CEC device ready with logical address: ");
    Serial.println(cec.getLogicalAddress(), HEX);
    Serial.println("Bridge is active. Send CEC commands to control Kenwood device.");
}

void loop() {
    // Run the CEC state machine
    cec.run();
    
    // Handle serial commands for testing
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input == "test") {
            Serial.println("Testing Kenwood commands...");
            kenwood.powerOn();
            delay(1000);
            kenwood.selectInput(CD);
            delay(1000);
            kenwood.startCD();
            delay(2000);
            kenwood.stopCD();
        } else if (input == "status") {
            Serial.print("CEC ready: ");
            Serial.println(cec.isReady() ? "Yes" : "No");
            Serial.print("CEC logical address: 0x");
            Serial.println(cec.getLogicalAddress(), HEX);
        }
    }
    
    delay(10); // Small delay to prevent overwhelming the loop
}