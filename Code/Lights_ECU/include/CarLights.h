#ifndef CAR_LIGHTS_H
#define CAR_LIGHTS_H

#include <Arduino.h>
#include "mcp_can.h"

class CarLights {
public:
    CarLights();
    void begin();

private:
    static const int CAN_CS_PIN = 5;
    static const int LED_LEFT_PIN = 13;
    static const int LED_RIGHT_PIN = 12;
    static const int LED_HEAD_PIN = 14;
    static const unsigned long BLINK_INTERVAL = 500;

    MCP_CAN _can;

    volatile bool _leftBlinkerEnabled;
    volatile bool _rightBlinkerEnabled;
    volatile bool _leftBlinkerState;
    volatile bool _rightBlinkerState;
    volatile bool _headlights;

    unsigned long _previousMillisLeft;
    unsigned long _previousMillisRight;

    TaskHandle_t _processingTaskHandle;
    TaskHandle_t _senderTaskHandle;

    void handleCANFrame(unsigned long rxId, unsigned char* data, byte len);

    static void processingTaskThunk(void* pvParameters);
    static void senderTaskThunk(void* pvParameters);

    void processingTask();
    void senderTask();
};

#endif
