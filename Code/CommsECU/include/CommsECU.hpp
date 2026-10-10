#ifndef COMMSECU_HPP
#define COMMSECU_HPP

#include <Arduino.h>
#include <eMessageID.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ICommsECU.hpp>

constexpr uint8_t MAXBYTES = 5;

class CommsECU : public ICommsECU
{
public:
    CommsECU(const char *paramSsid, const char *paramPassword, const char *paramBroker, const int paramPort);
    void Maintain() override;
    void MQTTSubscribe(const char *paramTopic) override;
    void MQTTPublish(const char *paramTopic, const uint8_t *paramPayload, unsigned int size) override;
    uint8_t GetCommand() override;
    uint16_t GetCommandID() override;
    void ResetCommand() override;
};

#endif