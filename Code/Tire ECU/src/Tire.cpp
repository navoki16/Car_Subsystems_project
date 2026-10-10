#include <Arduino.h>
#include <Tire.h>

Tire::Tire(uint8_t paramPin)
{
    this->pin = paramPin;
    pinMode((this->pin), INPUT);
}

uint16_t Tire::GetPressure()
{
    uint16_t tirePressure = map(analogRead((this->pin)), 0 , 4095, 10, 30);

    return tirePressure;
}