#include "superloopcommon.hpp"
#include "superloopconfig.hpp"

#include <atomic>
#include <cstdint>
#include <cstddef>

namespace SuperLoopCommon
{
    static std::atomic<uint64_t> tickTime{0};


    void increaseTickTime()
    {
        tickTime.fetch_add(1);
    }

    void IRQHandler()
    {
        increaseTickTime();
    }


    uint64_t getTickTime()
    {
        return tickTime.load();
    }


    bool delay(uint32_t milliseconds)
    {
        static uint64_t startTick = 0;
        static bool waiting = false;

        if (!waiting)
        {
            startTick = getTickTime();

            waiting = true;

            return false;
        }

        uint64_t elapsedTicks =
            getTickTime() - startTick;

        uint64_t requiredTicks =
            (static_cast<uint64_t>(milliseconds) *
             SuperLoopConfig::getTicksPerSecond()) / 1000;

        if (elapsedTicks >= requiredTicks)
        {
            waiting = false;

            return true;
        }

        return false;
    }

    uint16_t calculateCRC16(
        const uint8_t* data,
        size_t length
    )
    {
        uint16_t crc = 0xFFFF;

        for (size_t i = 0; i < length; i++)
        {
            crc ^= data[i];

            for (int bit = 0; bit < 8; bit++)
            {
                if (crc & 0x0001)
                {
                    crc = (crc >> 1) ^ 0xA001;
                }
                else
                {
                    crc >>= 1;
                }
            }
        }

        return crc;
    }


    bool validateCRC16(
        const uint8_t* data,
        size_t length,
        uint16_t expectedCRC
    )
    {
        return calculateCRC16(data, length) == expectedCRC;
    }

    const char* taskErrorToString(TaskError error)
    {
        switch (error)
        {
            case TaskError::NONE:
                return "NONE";

            case TaskError::REGISTRATION_FAILED:
                return "REGISTRATION_FAILED";

            case TaskError::INVALID_TASK_ID:
                return "INVALID_TASK_ID";

            case TaskError::CAPACITY_EXCEEDED:
                return "CAPACITY_EXCEEDED";

            case TaskError::TICK_NOT_ADVANCING:
                return "TICK_NOT_ADVANCING";

            case TaskError::SCHEDULER_LATE:
                return "SCHEDULER_LATE";

            case TaskError::MISSED_EXECUTION:
                return "MISSED_EXECUTION";

            case TaskError::TASK_EXCEPTION:
                return "TASK_EXCEPTION";

            case TaskError::TASK_TIMEOUT:
                return "TASK_TIMEOUT";

            case TaskError::DEPENDENCY_NOT_READY:
                return "DEPENDENCY_NOT_READY";

            case TaskError::TASK_DISABLED:
                return "TASK_DISABLED";

            case TaskError::INVALID_TASK_STATE:
                return "INVALID_TASK_STATE";

            default:
                return "UNKNOWN_ERROR";
        }
    }
}




