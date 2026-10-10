#include <Arduino.h>
#include <Engine.h>
#include <mcp_can.h>
#include <eMessageID.h>
#include <SPI.h>

constexpr uint32_t BAUDRATE = 115200;
constexpr uint8_t BUFFER_SIZE = 1;
constexpr uint16_t INTERVAL = 2000;
constexpr uint16_t STACK_DEPTH = 1024;

MCP_CAN CAN0(5);
Engine *engine;
byte data[BUFFER_SIZE];
unsigned long rxId;
unsigned long currentTime = 0;
byte len = 0;
byte rxBuf[BUFFER_SIZE];

void TransmitCAN(uint16_t paramID, uint8_t *buffer)
{
    byte msg = CAN0.sendMsgBuf(paramID, len, BUFFER_SIZE, buffer);
    if (msg != CAN_OK)
    {
        Serial.println("message send failed");
    }
}

void ReadCAN(void *parameter)
{
    while (true)
    {
        if (CAN0.checkReceive() == CAN_MSGAVAIL)
        {
            CAN0.readMsgBuf(&rxId, &len, rxBuf);
            switch (rxId)
            {
            case BRAKE_PRESSED:
                engine->StopEngine();

                break;
            case ENGINE_SET_RPM:
                engine->EngineControl(*rxBuf);

                break;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void SendEngineInfo(void *parameter)
{
    while (true)
    {
        if (millis() - currentTime >= INTERVAL)
        {
            *data = engine->GetTemp();
            TransmitCAN(ENGINE_TEMP, data);
            *data = engine->GetRPM();
            TransmitCAN(ENGINE_RPM, data);

            currentTime = millis();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    Serial.begin(BAUDRATE);
    engine = new Engine();

    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    {
        Serial.println("MCP2515 Initialized Successfully!");
    }
    else
    {
        Serial.println("Error Initializing MCP2515...");
    }
    CAN0.setMode(MCP_NORMAL);

    xTaskCreate(ReadCAN, "process incoming CAN messages", STACK_DEPTH, NULL, 1, NULL);
    xTaskCreate(SendEngineInfo, "send engine information every 2 seconds", STACK_DEPTH, NULL, 1, NULL);
}

void loop()
{
}