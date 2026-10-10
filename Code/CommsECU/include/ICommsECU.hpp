#ifndef ICOMMSECU_HPP
#define ICOMMSECU_HPP

#include <stdint.h>

class ICommsECU
{
public:
    virtual void Maintain() = 0;
    virtual void MQTTSubscribe(const char*) = 0;
    virtual void MQTTPublish(const char*, const uint8_t*, unsigned int) = 0;
    virtual uint8_t GetCommand() = 0;
    virtual uint16_t GetCommandID() = 0;
    virtual void ResetCommand() = 0;
};

#endif