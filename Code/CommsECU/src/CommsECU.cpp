#include <CommsECU.hpp>

WiFiClient espClient;
PubSubClient client(espClient);

byte command = UINT8_MAX;
uint16_t commandId = 0;

void callback(char *topic, byte *message, unsigned int length)
{
    String messageTemp;
    String receivedTopic = topic;

    for (int i = 0; i < length; i++)
    {
        messageTemp += (char)message[i];
    }

    if (receivedTopic == "esp32/brake")
    {
        command = 1;
        commandId = BRAKE_PRESSED;
    }
    else if (receivedTopic == "esp32/lights")
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
    else if (receivedTopic == "esp32/setRPM")
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

CommsECU::CommsECU(const char *paramSsid, const char *paramPassword, const char *paramBroker, const int paramPort)
{
    WiFi.begin(paramSsid, paramPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");

    // Configure the MQTT server
    client.setServer(paramBroker, paramPort);
    client.setCallback(callback);
}

void CommsECU::Maintain()
{
    if (!client.connected())
    {
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
    client.loop();
}