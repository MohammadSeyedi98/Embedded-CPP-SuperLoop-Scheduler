#ifndef SUPERLOOPCONFIG_HPP
#define SUPERLOOPCONFIG_HPP

#include <cstdint>

namespace SuperLoopConfig
{
    constexpr uint32_t MAX_TASKS = 10;


    bool configPerSecond(uint32_t ticksPerSecond);

    uint32_t getTicksPerSecond();

}

#endif
