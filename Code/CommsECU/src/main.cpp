#include <Arduino.h>
#include <eMessageID.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <mcp_can.h>
#include <LiquidCrystal_I2C.h>

constexpr int CAN_CS_PIN = 5;
constexpr int BAUDRATE = 115200;
constexpr uint8_t BUFFER_SIZE = 1;
constexpr uint16_t STACK_DEPTH = 2048;
constexpr uint16_t ERROR_TIMEOUT = 4000;
MCP_CAN CAN(CAN_CS_PIN);
WiFiClient espClient;
PubSubClient client(espClient);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char *ssid = "yourwifissid";
const char *password = "yourwifipassword";

// MQTT Broker details (using HiveMQ public broker)
const char *mqtt_broker = "mqtt.eclipseprojects.io";
const int mqtt_port = 1883;
const char *mqtt_topic_lights = "esp32/PJlights";
const char *mqtt_topic_temp = "esp32/PJtemp";
const char *mqtt_topic_pressure = "esp32/PJpressure";
const char *mqtt_topic_rpm = "esp32/PJrpm";
const char *mqtt_topic_brake = "esp32/PJbrake";
const char *mqtt_topic_setRPM = "esp32/PJsetRPM";
const char *mqtt_topic_nodeFail = "esp32/PJnodefailure";

byte command = UINT8_MAX;
uint16_t commandId = 0;
unsigned long engineErrorTime = 0;
unsigned long tireErrorTime = 0;
unsigned long lightErrorTime = 0;
bool prevError = false;
bool isError = false;

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    if (client.connect("ESP32Client"))
    {
      // Once connected, subscribe again
      client.subscribe(mqtt_topic_lights); // Subscribe to the topic
      client.subscribe(mqtt_topic_brake);
      client.subscribe(mqtt_topic_setRPM);
    }
  }
}

void CheckCANconnection(unsigned long errorTime)
{
  uint8_t sendBuffer[BUFFER_SIZE] = {1};
  if (millis() - errorTime >= ERROR_TIMEOUT)
  {
    isError = true;
    errorTime = millis();

    if (isError != prevError)
    {
      client.publish(mqtt_topic_nodeFail, sendBuffer, BUFFER_SIZE);
      prevError = isError;
    }
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  String messageTemp;
  String receivedTopic = topic;

  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)message[i];
  }

  if (receivedTopic == "esp32/PJbrake")
  {
    command = 1;
    commandId = BRAKE_PRESSED;
  }
  else if (receivedTopic == "esp32/PJlights")
  {
    if (messageTemp == "LB")
    {
      command = 1;
      commandId = BLINKERS;
    }
    else if (messageTemp == "RB")
    {
      command = 2;
      commandId = BLINKERS;
    }
    else if (messageTemp == "HL")
    {
      command = 3;
      commandId = HEADLIGHTS;
    }
  }
  else if (receivedTopic == "esp32/PJsetRPM")
  {
    commandId = ENGINE_SET_RPM;
    if (messageTemp == "true")
    {
      command = true;
    }
    else if (messageTemp == "false")
    {
      command = false;
    }
  }
}

void ReceiveAndSend(void *parameter)
{
  unsigned long rxId;
  byte len = 0;
  byte rxBuf[BUFFER_SIZE];

  while (true)
  {
    if (!client.connected())
    { // Reconnect if the client disconnects
      reconnect();
    }
    CheckCANconnection(engineErrorTime);
    CheckCANconnection(tireErrorTime);
    CheckCANconnection(lightErrorTime);
    client.loop();
    if (CAN.checkReceive() == CAN_MSGAVAIL)
    {
      CAN.readMsgBuf(&rxId, &len, rxBuf);
      rxId &= 0x0f;
      switch (rxId)
      {
      case ENGINE_TEMP:
        client.publish(mqtt_topic_temp, rxBuf, BUFFER_SIZE);

        // Display Engine Temperature on the LCD
        lcd.setCursor(0, 0); // Set cursor to the first row

        lcd.print("T: ");
        if (rxBuf[0] <= 19)
        {
          lcd.print(" LOW  ");
        }
        else
        {
          lcd.print(rxBuf[0]); // Assuming 1-byte temperature data
          lcd.print(" C     ");
        }
        engineErrorTime = millis();
        isError = false;
        prevError = isError;
        break;

      case TIRE_PRESSURE:
        client.publish(mqtt_topic_pressure, rxBuf, BUFFER_SIZE);

        // Display Tire Pressure on the LCD
        lcd.setCursor(0, 1); // Set cursor to the second row
        lcd.print("P: ");
        if (rxBuf[0] <= 19)
        {
          lcd.print(" LOW  ");
        }
        else
        {
          lcd.print(rxBuf[0]); // Assuming 1-byte pressure data
          lcd.print(" PSI ");
        }
        tireErrorTime = millis();
        isError = false;
        prevError = isError;
        break;

      case ENGINE_RPM:
        client.publish(mqtt_topic_rpm, rxBuf, BUFFER_SIZE);
        break;

      case HEADLIGHTS:
        lightErrorTime = millis();
        isError = false;
        prevError = isError;
        break;
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void CANmessanger(void *parameter)
{
  while (true)
  {
    if (command < UINT8_MAX)
    {
      byte data[BUFFER_SIZE] = {command};
      CAN.sendMsgBuf(commandId, 0, BUFFER_SIZE, data);
      Serial.print("Sent command: ");
      Serial.println(command, HEX);
      command = UINT8_MAX;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(BAUDRATE);

  if (CAN_OK != CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ))
  {
    Serial.println("CAN BUS init failed");
    while (1)
      ;
  }
  CAN.setMode(MCP_NORMAL);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Configure the MQTT server
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // Connect to the MQTT broker
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("Connected to MQTT");
      client.setKeepAlive(60);
      client.subscribe(mqtt_topic_lights, 2); // Subscribe to the topic
      client.subscribe(mqtt_topic_brake, 2);
      client.subscribe(mqtt_topic_setRPM, 2);
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("System Ready");

  xTaskCreate(ReceiveAndSend, "receive and act according to message received", STACK_DEPTH, NULL, 1, NULL);
  xTaskCreate(CANmessanger, "pass messages through CAN", STACK_DEPTH, NULL, 1, NULL);
}

void loop()
{
}