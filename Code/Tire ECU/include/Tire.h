#ifndef TIRE_H
#define TIRE_H

#include <ITire.h>

class Tire : public ITire
{
private:
    uint16_t pressure;
    uint8_t pin;

public:
    Tire(uint8_t paramPin);
    uint16_t GetPressure() override;

};

#endif