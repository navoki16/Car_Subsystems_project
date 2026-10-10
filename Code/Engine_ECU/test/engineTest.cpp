#include <Engine.h>
#include "gtest/gtest.h"

class EngineTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(EngineTest, throttle_up)
{
    Engine engine;
    engine.EngineControl(true);
    EXPECT_EQ(1, engine.GetRPM());
    EXPECT_EQ(60, engine.GetTemp());
}
TEST_F(EngineTest, throttle_up_max)
{
    Engine engine;
    engine.EngineControl(true);
    engine.EngineControl(true);
    engine.EngineControl(true);
    engine.EngineControl(true);
    engine.EngineControl(true);
    engine.EngineControl(true);
    engine.EngineControl(true);
    EXPECT_EQ(6, engine.GetRPM());
    EXPECT_EQ(100, engine.GetTemp());
}
TEST_F(EngineTest, throttle_down)
{
    Engine engine;
    engine.EngineControl(true);
    engine.EngineControl(true);
    engine.EngineControl(false);
    EXPECT_EQ(1, engine.GetRPM());
    EXPECT_EQ(60, engine.GetTemp());
}
TEST_F(EngineTest, throttle_down_min)
{
    Engine engine;
    engine.EngineControl(true);
    engine.EngineControl(false);
    engine.EngineControl(false);
    EXPECT_EQ(0, engine.GetRPM());
    EXPECT_EQ(60, engine.GetTemp());
}
TEST_F(EngineTest, brake)
{
    Engine engine;
    engine.EngineControl(true);
    engine.EngineControl(true);
    engine.StopEngine();
    EXPECT_EQ(0, engine.GetRPM());
    EXPECT_EQ(60, engine.GetTemp());
}