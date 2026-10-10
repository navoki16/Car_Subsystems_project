#ifndef ITIRE_H
#define ITIRE_H
#include <stdint.h>

class ITire
{
public:
    virtual uint16_t GetPressure() = 0;
};
#endif