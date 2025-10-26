# KenwoodXS Arduino Library

## Overview

The KenwoodXS library provides a simple interface to control Kenwood audio equipment using the proprietary Kenwood XS (System Control) protocol. This library allows Arduino-based projects to send commands to compatible Kenwood receivers, tape decks, and CD players.

## Hardware Setup

The KenwoodXS protocol uses two signal lines:
- **CTRL** (Control/White wire): Controls the transmission timing
- **SDAT** (Serial Data/Red wire): Carries the actual command data
- **GND** (Ground/Black wire): Common ground connection

Connect these to your Arduino:
- CTRL pin to a digital output pin (default: pin 3)
- SDAT pin to a digital output pin (default: pin 2)  
- Ground wire to Arduino GND

## Usage Example

```cpp
#include <KenwoodXS.h>

// Create KenwoodXS instance with CTRL on pin 3, SDAT on pin 2
KenwoodXS kenwood(3, 2);

void setup() {
    Serial.begin(9600);
    kenwood.begin();
    kenwood.setDebugOutput(true);
}

void loop() {
    // Power on the receiver
    kenwood.powerOn();
    delay(1000);
    
    // Select CD input
    kenwood.selectInput(CD);
    delay(1000);
    
    // Start playing CD
    kenwood.playCD();
    delay(5000);
    
    // Stop CD
    kenwood.stopCD();
    delay(1000);
}
```

## API Reference

### Constructor
- `KenwoodXS(int ctrlPin, int sdatPin)` - Create instance with specified pins

### Setup Methods
- `begin()` - Initialize the pins and prepare for communication
- `setDebugOutput(bool enabled)` - Enable/disable debug messages via Serial

### Power Control
- `powerOn()` - Turn on the receiver
- `powerOff()` - Turn off the receiver

### Input Selection
- `selectInput(KenwoodXSCommand input)` - Select input source (CD, TAPE_1, VIDEO_1, etc.)

### Transport Controls
- `playCD()` - Play/pause CD
- `startCD()` - Start CD (command 222)
- `stopCD()` - Stop CD
- `playTapeA()` - Play Tape A
- `stopTapeA()` - Stop Tape A
- `playTapeB()` - Play Tape B  
- `stopTapeB()` - Stop Tape B

### Raw Commands
- `sendCommand(byte command)` - Send raw command byte
- `sendCommand(KenwoodXSCommand command)` - Send command using enum
- `tryAllCommands(unsigned long delayMs)` - Test all commands for debugging

### Callbacks
- `onCommandSentCallback(OnCommandSentCallback callback)` - Set callback for when commands are sent

## Table of Results

The below table makes pretty the data recorded at the top of KenwoodXS.ino

| Button | Received Code |
| --- | --- |
| **Miscellaneous** |
| `POWER` (On) | 181 |
| `POWER` (Off) | 185 |
| `BAND` (Tuner AM/FM)<sup>1</sup> | 123 |
| `+10` | 164 |
| **Input** |
| `TAPE 2` (toggle on) | 20 |
| `TAPE 2` (toggle off) | 24 |
| `VIDEO 2` | 113 |
| `PHONO` | 115 |
| `TAPE 1` | 121 |
| `VIDEO 1` | 122 |
| `TUNER`<sup>1</sup> | 123 |
| `CD` | 125 |
| **Tape A** |
| `>>` (fast forward) | 67 |
| `[]` (stop) | 69 |
| `>` (play) | 71 |
| `*` (record) | 73 |
| `<<` (rewind) | 75 |
| `\|\|` (pause) | 77 |
| `<` (rewind slow?) | 79 |
| **Tape B** |
| `>>` | 66 |
| `[]` | 68 |
| `>` | 70 |
| `*` | 72 |
| `<<` | 74 |
| `\|\|` | 76 |
| `<` | 78 |
| **CD** | 
| `>/\|\|` (toggle play/pause | 194 |
| `[]` | 198 |
| `DISC` (change disc) | 174 |
| `>>`<sup>2</sup> | 161 |
| `<<`<sup>2</sup> | 169 |
| `>>\|`<sup>2</sup> | 192 |
| `\|<<`<sup>2</sup> | 200 |
| (stop scrubbing/release fwd/rwd) | 177 |

<sup>1</sup> The behavior of these buttons appear to be identical from this code extraction... but the actual behavior on the device changes to the `TUNER` input if the `TUNER` button is pressed, and switches between AM and FM if the `BAND` button is pressed. While they output the same code, I'm not sure how the receiver would be able to tell that the `BAND` button was pressed versus the `TUNER` button? There must be a bug in the code somewhere...

<sup>2</sup> All of these buttons send their listed code on press (and hold) of the button, and then send the stop code (177) on release of the button. I haven't tested this yet, as I do not own any Kenwood CD players or devices that support Kenwood XS other than this receiver, but I think I can safely suspect that this is meant to "scrub" through the CD (`>>` and `<<` through the current track with volume, `>>|` and `|<<` track by track).


