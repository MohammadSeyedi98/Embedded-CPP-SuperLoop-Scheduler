#ifndef SUPERLOOPCOMMON_HPP
#define SUPERLOOPCOMMON_HPP

#include <cstdint>
#include <cstddef>

namespace SuperLoopCommon
{
    enum class TaskStatus
    {
        INVALID,
        REGISTERED,
        READY,
        RUNNING,
        ERROR
    };

    enum class TaskError
    {
        NONE = 0,

        REGISTRATION_FAILED,
        INVALID_TASK_ID,
        CAPACITY_EXCEEDED,

        TICK_NOT_ADVANCING,
        SCHEDULER_LATE,
        MISSED_EXECUTION,

        TASK_EXCEPTION,
        TASK_TIMEOUT,
        DEPENDENCY_NOT_READY,

        TASK_DISABLED,
        INVALID_TASK_STATE
    };

    const char* taskErrorToString(TaskError error);

    void increaseTickTime();

    void IRQHandler();

    uint64_t getTickTime();

    bool delay(uint32_t milliseconds);

    uint16_t calculateCRC16( const uint8_t* data, size_t length);

    bool validateCRC16(const uint8_t* data, size_t length, uint16_t expectedCRC);

}

#endif
