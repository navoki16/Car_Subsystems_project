#include <Engine.h>

constexpr uint8_t MAX_RPM = 6;

Engine::Engine()
{
    temperature = 0;
    RPM = 0;
}

void Engine::EngineControl(bool throttle)
{
    switch (throttle)
    {
    case true:
        if (RPM < MAX_RPM)
        {
            RPM ++;
        }

        break;

    case false:
        if (RPM > 0)
        {
            RPM --;
        }

        break;
    }
}

void Engine::StopEngine()
{
    RPM = 0;
}

uint8_t Engine::GetTemp()
{
    if (RPM < 2)
    {
        temperature = 60;
    }
    else if ((RPM >= 2) && (RPM < 4))
    {
        temperature = 80;
    }
    else
    {
        temperature = 100;
    }

    return temperature;
}

uint8_t Engine::GetRPM()
{
    return RPM;
}