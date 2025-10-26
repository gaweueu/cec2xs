# CEClient Library

A comprehensive Arduino library for HDMI Consumer Electronics Control (CEC) communication. This library enables Arduino-based projects to communicate with HDMI CEC devices, allowing control and monitoring of modern HDMI-connected equipment.

## Overview

The CEClient library provides a complete implementation of the HDMI CEC protocol, allowing Arduino devices to:
- Receive CEC commands from TVs, receivers, and other HDMI devices
- Send CEC commands to control HDMI devices
- Act as various CEC device types (TV, playback device, audio system, etc.)
- Monitor all CEC bus traffic
- Handle automatic logical address allocation

## Features

- ✅ **Full CEC Protocol Support**: Complete implementation of HDMI CEC specification
- ✅ **Multiple Device Types**: Support for TV, playback device, audio system, tuner, and recording device
- ✅ **Automatic Address Allocation**: Handles logical address negotiation automatically
- ✅ **Promiscuous Mode**: Monitor all CEC bus traffic
- ✅ **Monitor Mode**: Receive-only mode for passive monitoring
- ✅ **Callback System**: Custom handlers for received messages and transmission events
- ✅ **Hardware Abstraction**: Works on Arduino Uno, Nano, and compatible boards
- ✅ **Non-blocking Operation**: State machine-based implementation for smooth operation

## Hardware Requirements

### Arduino Compatibility
- Arduino Uno, Nano, Pro Mini (ATmega328P)
- Arduino Mega (ATmega2560)
- STM32-based boards (experimental)
- Minimum 2 digital I/O pins

### CEC Connection
- HDMI CEC signal line (pin 13 on HDMI connector)
- Ground connection to HDMI ground
- Optional: 5V pull-up resistor (4.7kΩ) on CEC line for improved signal integrity

### Wiring Diagram
```
Arduino Uno/Nano      HDMI Connector
┌─────────────┐       ┌─────────────┐
│ Pin 2 (IN)  │──────→│ Pin 13 (CEC)│
│ Pin 3 (OUT) │──────→│ Pin 13 (CEC)│  (Optional: same as input)
│ GND         │──────→│ Pin 17 (GND)│
│ 5V          │       │ +5V via 4.7kΩ│  (Optional pull-up)
└─────────────┘       └─────────────┘
```

## Installation

### PlatformIO
1. Copy the `CEClient` folder to your project's `lib/` directory
2. Include in your code: `#include <CEClient.h>`

### Arduino IDE
1. Copy the `CEClient` folder to your Arduino libraries directory
2. Restart Arduino IDE
3. Include in your sketch: `#include <CEClient.h>`

## Quick Start

### Basic CEC Monitor
```cpp
#include <Arduino.h>
#include <CEClient.h>

#define CEC_PHYSICAL_ADDRESS 0x1000
#define CEC_INPUT_PIN        2
#define CEC_OUTPUT_PIN       3

CEClient cec(CEC_PHYSICAL_ADDRESS, CEC_INPUT_PIN, CEC_OUTPUT_PIN);

void setup() {
    Serial.begin(115200);
    
    cec.begin();
    cec.setPromiscuous(true);  // Monitor all CEC messages
    cec.setMonitorMode(true);  // Receive-only mode
    
    Serial.println("CEC Monitor Started");
}

void loop() {
    cec.run();  // Must be called continuously
}
```

### CEC Device with Custom Handler
```cpp
#include <Arduino.h>
#include <CEClient.h>

#define CEC_PHYSICAL_ADDRESS 0x2000
#define CEC_INPUT_PIN        2
#define CEC_OUTPUT_PIN       3

CEClient cec(CEC_PHYSICAL_ADDRESS, CEC_INPUT_PIN, CEC_OUTPUT_PIN);

void onCECReceived(int source, int dest, unsigned char* buffer, int count) {
    Serial.print("CEC: ");
    Serial.print(source, HEX);
    Serial.print(" -> ");
    Serial.print(dest, HEX);
    Serial.print(" [");
    
    for (int i = 0; i < count; i++) {
        Serial.print("0x");
        Serial.print(buffer[i], HEX);
        if (i < count - 1) Serial.print(", ");
    }
    Serial.println("]");
    
    // Handle specific commands
    if (count > 0) {
        switch (buffer[0]) {
            case 0x36:  // Standby
                Serial.println("Standby command received");
                break;
            case 0x04:  // Image View On
                Serial.println("Power on command received");
                break;
            case 0x44:  // User Control Pressed
                if (count > 1) {
                    Serial.print("User button: 0x");
                    Serial.println(buffer[1], HEX);
                }
                break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    cec.begin(CEC_LogicalDevice::CDT_PLAYBACK_DEVICE);
    cec.onReceiveCallback(onCECReceived);
    
    // Wait for CEC device to be ready
    while (!cec.isReady()) {
        cec.run();
        delay(100);
    }
    
    Serial.print("CEC ready with logical address: 0x");
    Serial.println(cec.getLogicalAddress(), HEX);
}

void loop() {
    cec.run();
    delay(10);
}
```

## API Reference

### Constructor
```cpp
CEClient(int physicalAddress, int inputPin, int outputPin = -1)
```
- **physicalAddress**: 16-bit physical address (e.g., 0x1000)
- **inputPin**: Arduino pin for CEC signal input
- **outputPin**: Arduino pin for CEC signal output (optional, defaults to inputPin)

### Initialization Methods
```cpp
void begin(CEC_DEVICE_TYPE type = CDT_PLAYBACK_DEVICE)
```
Initialize the CEC device with specified type.

**Device Types:**
- `CDT_TV` - Television
- `CDT_RECORDING_DEVICE` - Recording device (DVR, etc.)
- `CDT_TUNER` - Tuner device
- `CDT_PLAYBACK_DEVICE` - Playback device (default)
- `CDT_AUDIO_SYSTEM` - Audio system/receiver
- `CDT_OTHER` - Other device type

### Status Methods
```cpp
bool isReady()
```
Returns `true` when the CEC device has completed initialization and logical address allocation.

```cpp
int getLogicalAddress()
```
Returns the allocated logical address (0-15), or -1 if not ready.

### Communication Methods
```cpp
bool write(int targetAddress, unsigned char* buffer, int count)
```
Send a CEC message to the specified target address.
- **targetAddress**: Logical address of target device (0-15)
- **buffer**: Message data to send
- **count**: Number of bytes to send
- **Returns**: `true` if transmission started successfully

### Configuration Methods
```cpp
void setPromiscuous(bool promiscuous)
```
Enable/disable promiscuous mode. When enabled, receives all CEC messages regardless of destination address.

```cpp
void setMonitorMode(bool monitorMode)
```
Enable/disable monitor mode. When enabled, the device only receives messages and never transmits responses.

### Callback Methods
```cpp
void onReceiveCallback(OnReceiveCallbackFunction callback)
```
Set callback function for received CEC messages.
**Callback signature:**
```cpp
void callback(int source, int dest, unsigned char* buffer, int count)
```

```cpp
void onTransmitCompleteCallback(OnTransmitCompleteCallbackFunction callback)
```
Set callback function for transmission completion.
**Callback signature:**
```cpp
void callback(bool success)
```

### Main Loop Method
```cpp
void run()
```
**CRITICAL**: Must be called continuously in your main loop. Handles the CEC state machine and all protocol timing.

## Common CEC Commands

### Power Control
- `0x36` - Standby (power off)
- `0x04` - Image View On (power on)
- `0x0D` - Text View On (power on)

### Remote Control
- `0x44` - User Control Pressed
  - `0x44` - Play
  - `0x45` - Stop  
  - `0x46` - Record
  - `0x47` - Rewind
  - `0x48` - Fast Forward
  - `0x4B` - Pause/Play
  - `0x00` - Select
  - `0x01` - Up
  - `0x02` - Down
  - `0x03` - Left
  - `0x04` - Right

### System Information
- `0x83` - Give Physical Address
- `0x84` - Report Physical Address
- `0x8F` - Give Device Power Status
- `0x90` - Report Power Status

### Audio Control
- `0x82` - Active Source
- `0x9D` - Inactive Source
- `0x71` - Give Audio Status
- `0x7A` - Report Audio Status

## Physical Addresses

Physical addresses represent the HDMI topology:
- `0x0000` - TV (root)
- `0x1000` - HDMI port 1 on TV
- `0x2000` - HDMI port 2 on TV
- `0x3000` - HDMI port 3 on TV
- `0x1100` - HDMI port 1 on device connected to TV port 1

## Logical Addresses

Logical addresses are automatically allocated based on device type:
- `0x0` - TV
- `0x1-0x3` - Recording devices
- `0x3-0x4` - Tuners
- `0x4-0x8` - Playback devices
- `0x5` - Audio system
- `0xE` - Free use
- `0xF` - Broadcast/unregistered

## Example Projects

### 1. TV Power Control Bridge
Monitor TV power commands and control external devices:
```cpp
void onCECReceived(int source, int dest, unsigned char* buffer, int count) {
    if (count == 1 && buffer[0] == 0x36) {
        // TV went to standby, turn off other devices
        digitalWrite(RELAY_PIN, LOW);
    }
    if (count == 1 && buffer[0] == 0x04) {
        // TV powered on, turn on other devices
        digitalWrite(RELAY_PIN, HIGH);
    }
}
```

### 2. Custom Remote Control
Send CEC commands to control devices:
```cpp
void sendPlayCommand() {
    unsigned char cmd[] = {0x44, 0x44};  // User Control Pressed: Play
    cec.write(0x4, cmd, 2);  // Send to playback device
}

void sendStandbyCommand() {
    unsigned char cmd[] = {0x36};  // Standby
    cec.write(0x0, cmd, 1);  // Send to TV
}
```

### 3. CEC Bus Analyzer
Log all CEC traffic for debugging:
```cpp
void onCECReceived(int source, int dest, unsigned char* buffer, int count) {
    Serial.print(millis());
    Serial.print("ms CEC: ");
    Serial.print(source, HEX);
    Serial.print(" -> ");
    Serial.print(dest, HEX);
    Serial.print(" : ");
    
    for (int i = 0; i < count; i++) {
        Serial.print("0x");
        if (buffer[i] < 0x10) Serial.print("0");
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}
```

## Troubleshooting

### Device Not Ready
**Problem**: `isReady()` returns false indefinitely
**Solutions**:
- Check CEC wiring connections
- Verify CEC is enabled on connected devices
- Try different physical address
- Add timeout in initialization loop

### No Messages Received
**Problem**: Callback never triggers
**Solutions**:
- Enable promiscuous mode: `setPromiscuous(true)`
- Check that `run()` is called continuously in loop
- Verify CEC signal integrity
- Test with known CEC traffic (TV remote)

### Transmission Fails
**Problem**: `write()` returns false or messages not received
**Solutions**:
- Ensure device is ready before transmitting
- Verify target logical address exists
- Check if monitor mode is disabled for transmission
- Use transmission complete callback to verify success

### Timing Issues
**Problem**: Intermittent communication problems
**Solutions**:
- Call `run()` more frequently (shorter delays in loop)
- Avoid long blocking operations in callbacks
- Consider hardware pull-up resistor on CEC line

## Advanced Usage

### Custom Device Types
```cpp
// Create a custom audio system
cec.begin(CEC_LogicalDevice::CDT_AUDIO_SYSTEM);
```

### Transmission Confirmation
```cpp
void onTransmitComplete(bool success) {
    if (success) {
        Serial.println("Message sent successfully");
    } else {
        Serial.println("Transmission failed");
    }
}

void setup() {
    cec.onTransmitCompleteCallback(onTransmitComplete);
}
```

### Filtering Messages
```cpp
void onCECReceived(int source, int dest, unsigned char* buffer, int count) {
    // Only process messages from TV (source 0)
    if (source != 0x0) return;
    
    // Only process messages to us or broadcast
    int myAddress = cec.getLogicalAddress();
    if (dest != myAddress && dest != 0xF) return;
    
    // Process the message
    handleCECCommand(buffer, count);
}
```

## Technical Details

### Protocol Implementation
- Implements HDMI CEC 1.4 specification
- Automatic bit timing and signal level detection
- Collision detection and retry logic
- Proper arbitration handling

### Memory Usage
- RAM: ~200 bytes for state machine and buffers
- Flash: ~8KB for complete library
- No dynamic memory allocation

### Performance
- Non-blocking state machine operation
- Microsecond-precision timing
- Handles CEC bus speeds up to 400kHz

## License

This library is based on the work of multiple contributors:
- Original CEC implementation by various authors
- Arduino adaptation and CEClient wrapper
- Contributions from the open-source community

## References

- [HDMI CEC Specification](https://www.hdmi.org/)
- [CEC-O-Matic Online Decoder](http://www.cec-o-matic.com/)
- [Arduino CEC Libraries Comparison](https://github.com/topics/hdmi-cec)

## Contributing

Contributions are welcome! Please:
1. Test thoroughly on hardware
2. Follow existing code style
3. Update documentation for new features
4. Submit pull requests with clear descriptions

## Support

For issues and questions:
1. Check the troubleshooting section
2. Review example code
3. Test with minimal examples
4. Report bugs with complete setup details
