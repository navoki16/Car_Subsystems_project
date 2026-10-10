#include "CarLights.h"
#include <SPI.h>

CarLights::CarLights()
: _can(CAN_CS_PIN),
  _leftBlinkerEnabled(false),
  _rightBlinkerEnabled(false),
  _leftBlinkerState(false),
  _rightBlinkerState(false),
  _headlights(false),
  _previousMillisLeft(0),
  _previousMillisRight(0),
  _processingTaskHandle(nullptr),
  _senderTaskHandle(nullptr)
{
}

void CarLights::begin() {
    Serial.begin(115200);
    SPI.begin(18, 19, 23, CAN_CS_PIN);

    if (CAN_OK != _can.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ)) {
        Serial.println("CAN BUS init failed");
        while (true) {
            delay(100);
        }
    }
    _can.setMode(MCP_NORMAL);
    Serial.println("CAN BUS init OK!");

    pinMode(LED_LEFT_PIN,  OUTPUT);
    pinMode(LED_RIGHT_PIN, OUTPUT);
    pinMode(LED_HEAD_PIN,  OUTPUT);

    digitalWrite(LED_LEFT_PIN,  LOW);
    digitalWrite(LED_RIGHT_PIN, LOW);
    digitalWrite(LED_HEAD_PIN,  LOW);

    xTaskCreatePinnedToCore(
        processingTaskThunk,
        "TaskProcessing",
        4096,
        this,
        1,
        &_processingTaskHandle,
        0
    );

    xTaskCreatePinnedToCore(
        senderTaskThunk,
        "TaskCANSender",
        2048,
        this,
        1,
        &_senderTaskHandle,
        1
    );
}

void CarLights::processingTaskThunk(void* pvParameters) {
    CarLights* self = static_cast<CarLights*>(pvParameters);
    self->processingTask();
}

void CarLights::senderTaskThunk(void* pvParameters) {
    CarLights* self = static_cast<CarLights*>(pvParameters);
    self->senderTask();
}

void CarLights::processingTask() {
    while (true) {
        if (CAN_MSGAVAIL == _can.checkReceive()) {
            unsigned char len = 0;
            unsigned char buf[8];
            unsigned long rxId;

            _can.readMsgBuf(&rxId, &len, buf);
            handleCANFrame(rxId, buf, len);
        }

        unsigned long currentMillis = millis();

        if (_leftBlinkerEnabled) {
            if (currentMillis - _previousMillisLeft >= BLINK_INTERVAL) {
                _previousMillisLeft = currentMillis;
                _leftBlinkerState = !_leftBlinkerState;
                digitalWrite(LED_LEFT_PIN, _leftBlinkerState ? HIGH : LOW);
            }
        }

        if (_rightBlinkerEnabled) {
            if (currentMillis - _previousMillisRight >= BLINK_INTERVAL) {
                _previousMillisRight = currentMillis;
                _rightBlinkerState = !_rightBlinkerState;
                digitalWrite(LED_RIGHT_PIN, _rightBlinkerState ? HIGH : LOW);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void CarLights::senderTask() {
    unsigned char data[1] = {0x01};

    while (true) {
        byte sndStat = _can.sendMsgBuf(0x07, 0, 1, data);
        if (sndStat == CAN_OK) {
            Serial.println("CAN message sent: ID = 0x07, Data = {0x01}");
        } else {
            Serial.println("Error sending CAN message");
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void CarLights::handleCANFrame(unsigned long rxId, unsigned char* data, byte len) {
    if (len == 0) return;
    unsigned char command = data[0];

    Serial.print("Received command: 0x");
    Serial.println(command, HEX);
    Serial.print("ID: 0x");
    Serial.println(rxId, HEX);

    switch (rxId) {
        case 0x06:
            if (command == 0x01) {
                _leftBlinkerEnabled = !_leftBlinkerEnabled;
                if (!_leftBlinkerEnabled) {
                    _leftBlinkerState = false;
                    digitalWrite(LED_LEFT_PIN, LOW);
                }
                Serial.println("Toggled Left Blinker");
            } else if (command == 0x02) {
                _rightBlinkerEnabled = !_rightBlinkerEnabled;
                if (!_rightBlinkerEnabled) {
                    _rightBlinkerState = false;
                    digitalWrite(LED_RIGHT_PIN, LOW);
                }
                Serial.println("Toggled Right Blinker");
            }
            break;

        case 0x07:
            if (command == 0x03) {
                _headlights = !_headlights;
                digitalWrite(LED_HEAD_PIN, _headlights ? HIGH : LOW);
                Serial.println("Toggled Headlights");
            }
            break;

        default:
            break;
    }
}
