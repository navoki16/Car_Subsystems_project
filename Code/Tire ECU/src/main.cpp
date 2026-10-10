#include <Arduino.h>
#include <Tire.h>
#include <mcp_can.h>
#include <eMessageID.h>

constexpr uint32_t BAUDRATE = 115200;
constexpr uint8_t BUFFER_SIZE = 1;
constexpr uint16_t INTERVAL = 2000;
constexpr uint8_t TIRE_PIN = 35;
constexpr uint16_t STACK_DEPTH = 1024;

Tire *tire;
MCP_CAN CAN0(5);
byte data[BUFFER_SIZE];
unsigned long rxId;
unsigned long currentTime = 0;
byte len = 0;
byte rxBuf[BUFFER_SIZE];

void SendTirePressure(void *parameter)
{
  while (true)
  {
    if (millis() - currentTime >= INTERVAL)
    {
      *data = tire->GetPressure();
      Serial.println(*data);
      byte msg = CAN0.sendMsgBuf(TIRE_PRESSURE, 0, BUFFER_SIZE, data);
      if (msg != CAN_OK)
      {
        Serial.println("failed to send message");
      }

      currentTime = millis();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  tire = new Tire(TIRE_PIN);

  Serial.begin(BAUDRATE);
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
  {
    Serial.println("MCP2515 Initialized Successfully!");
  }
  else
  {
    Serial.println("Error Initializing MCP2515...");
  }
  CAN0.setMode(MCP_NORMAL);

  xTaskCreate(SendTirePressure, "send tire pressure every 2 seconds", STACK_DEPTH, NULL, 1, NULL);
}

void loop()
{
}