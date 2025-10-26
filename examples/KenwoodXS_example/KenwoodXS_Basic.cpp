#include <Arduino.h>
#include <KenwoodXS.h>

// Pin definitions - adjust these to match your hardware setup
const int CTRL_PIN = 3;  // Control pin (white wire)
const int SDAT_PIN = 2;  // Serial data pin (red wire)

// Create KenwoodXS instance
KenwoodXS kenwood(CTRL_PIN, SDAT_PIN);

void onCommandSent(byte command) {
    Serial.print("Command sent: ");
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
    
    Serial.println("KenwoodXS Library Example");
    Serial.println("Commands:");
    Serial.println("1-255: Send raw command");
    Serial.println("p: Power On");
    Serial.println("o: Power Off");
    Serial.println("c: Select CD input");
    Serial.println("t: Select Tape 1 input");
    Serial.println("v: Select Video 1 input");
    Serial.println("s: Play CD");
    Serial.println("r: Start CD");
    Serial.println("x: Stop CD");
    Serial.println("a: Play Tape A");
    Serial.println("z: Stop Tape A");
    Serial.println("999: Test all commands (slow)");
    Serial.println();

    // Initialize the KenwoodXS library
    kenwood.begin();
    kenwood.setDebugOutput(true);
    kenwood.onCommandSentCallback(onCommandSent);
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input.length() == 1) {
            char cmd = input.charAt(0);
            switch (cmd) {
                case 'p':
                    Serial.println("Sending Power On");
                    kenwood.powerOn();
                    break;
                case 'o':
                    Serial.println("Sending Power Off");
                    kenwood.powerOff();
                    break;
                case 'c':
                    Serial.println("Selecting CD input");
                    kenwood.selectInput(CD);
                    break;
                case 't':
                    Serial.println("Selecting Tape 1 input");
                    kenwood.selectInput(TAPE_1);
                    break;
                case 'v':
                    Serial.println("Selecting Video 1 input");
                    kenwood.selectInput(VIDEO_1);
                    break;
                case 's':
                    Serial.println("Playing CD");
                    kenwood.playCD();
                    break;
                case 'r':
                    Serial.println("Starting CD");
                    kenwood.startCD();
                    break;
                case 'x':
                    Serial.println("Stopping CD");
                    kenwood.stopCD();
                    break;
                case 'a':
                    Serial.println("Playing Tape A");
                    kenwood.playTapeA();
                    break;
                case 'z':
                    Serial.println("Stopping Tape A");
                    kenwood.stopTapeA();
                    break;
                default:
                    Serial.println("Unknown command");
                    break;
            }
        } else {
            long val = input.toInt();
            if (val == 999) {
                Serial.println("Testing all commands with 1 second delay...");
                kenwood.tryAllCommands(1000);
            } else if (val > 0 && val < 256) {
                Serial.print("Sending raw command: ");
                Serial.println(val);
                kenwood.sendCommand((byte)val);
            } else {
                Serial.println("Invalid input. Enter 1-255 for raw commands or single letters for preset commands.");
            }
        }
    }
}