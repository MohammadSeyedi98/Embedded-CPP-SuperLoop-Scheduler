#include "superloopconfig.hpp"

namespace SuperLoopConfig
{

    static uint32_t ticksPerSecond = 1000;

    static bool configured = false;


    bool configPerSecond(uint32_t value)
    {
        if (configured)
        {
            return false;
        }

        if (value == 0)
        {
            return false;
        }

        ticksPerSecond = value;

        configured = true;

        return true;
    }


    uint32_t getTicksPerSecond()
    {
        return ticksPerSecond;
    }


}
