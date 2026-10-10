#ifndef ENGINE_H
#define ENGINE_H

#include <IEngine.h>

class Engine : public IEngine
{
private:
    uint8_t temperature;
    uint8_t RPM;

public:
    Engine();
    void EngineControl(bool) override;
    void StopEngine() override;
    uint8_t GetTemp() override;
    uint8_t GetRPM() override;
};

#endif