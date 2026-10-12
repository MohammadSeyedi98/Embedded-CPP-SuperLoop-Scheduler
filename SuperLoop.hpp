#ifndef SUPERLOOP_HPP
#define SUPERLOOP_HPP

#include <cstdint>
#include <functional>
#include <string>

#include "superloopcommon.hpp"

namespace SuperLoop
{
    uint32_t registerTask(
        std::function<void()> workflow,
        uint32_t periodMs
    );

    bool startLoop();

    bool runOnce();

    SuperLoopCommon::TaskStatus getTaskStatus(
        uint32_t refId
    );

    int getTaskErrorCode(
        uint32_t refId
    );

    std::string getTaskErrorMessage(
        uint32_t refId
    );

    uint32_t getMissedExecutions(
        uint32_t refId
    );

    void resetScheduler();
}


#endif
