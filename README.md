# Purpose
This project implements an Automotive Electronic Control Unit (ECU) using the Arduino framework in C++. The ECU is responsible for monitoring vehicle signals, processing sensor data, and controlling actuators according to predefined logic.

# Features
- CAN communication
- Diagnostic output
- Sensor signal simulation
- Actuator control simulation
- Node-Red dashboard 

# Setup
## Hardware
- ESP32 Microcontroller
- CAN Transceiver module (MCP2515)

## Software
- PlatformIO on VSCode
- Libraries:
    - mcp_can
    - pubsubclient
 
# Usage
4 Microcontroller each connected through a CAN bus using the CAN transceiver module. Each microcontroller is a simulated ECU which constantly updates simulated data. The communications ECU sends these data through MQTT to a Node-Red dashboard to visualise the simulated data.
## Dashboard Connection
Start up a Node-Red dashboard which subscribe to the topics: esp32/PJlights, esp32/PJtemp, esp32/PJpressure, esp32/PJrpm, esp32/PJnodefailure
Create a brake button and a "set rpm" button in Node-Red dashboard which publishes to the topics: esp32/PJbrake, esp32/PJsetRPM
