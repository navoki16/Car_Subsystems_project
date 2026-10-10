#ifndef IENGINE_H
#define IENGINE_H

#include <stdint.h>

class IEngine
{
public:
    virtual void EngineControl(bool throttle) = 0;
    virtual void StopEngine() = 0;
    virtual uint8_t GetTemp() = 0;
    virtual uint8_t GetRPM() = 0;
};

#endif