#include "superloop.hpp"
#include "superloopconfig.hpp"

#include <vector>
#include <string>
#include <exception>

namespace SuperLoop
{
    struct Task
    {
        uint32_t refId;

        std::function<void()> workflow;

        uint32_t periodMs;
        uint64_t periodTicks;

        uint64_t lastRunTick;

        uint32_t missedExecutions;

        SuperLoopCommon::TaskStatus status;

        SuperLoopCommon::TaskError errorCode =
            SuperLoopCommon::TaskError::NONE;

        std::string errorMessage;
    };

    static std::vector<Task> tasks;

    static uint32_t nextRefId = 1;


    uint32_t registerTask(
        std::function<void()> workflow,
        uint32_t periodMs
    )
    {
    	if (tasks.size() >= SuperLoopConfig::MAX_TASKS)
        {
            return 0;
        }

        Task task;

        task.refId = nextRefId++;

        task.workflow = workflow;

        task.periodMs = periodMs;

        task.periodTicks =
            (static_cast<uint64_t>(periodMs) *
             SuperLoopConfig::getTicksPerSecond()) / 1000;

        task.lastRunTick =
            SuperLoopCommon::getTickTime();

        task.missedExecutions = 0;

        task.status =
            SuperLoopCommon::TaskStatus::REGISTERED;

        task.errorCode = SuperLoopCommon::TaskError::NONE;
        task.errorMessage.clear();

        tasks.push_back(task);

        return task.refId;

    }


    SuperLoopCommon::TaskStatus getTaskStatus(
        uint32_t refId
    )
    {
        for (const Task& task : tasks)
        {
            if (task.refId == refId)
            {
                return task.status;
            }
        }

        return SuperLoopCommon::TaskStatus::INVALID;
    }


    int getTaskErrorCode(uint32_t refId)
    {
        for (const Task& task : tasks)
        {
            if (task.refId == refId)
            {
            	return static_cast<int>(task.errorCode);
            }
        }

        return -1;
    }


    std::string getTaskErrorMessage(uint32_t refId)
    {
        for (const Task& task : tasks)
        {
            if (task.refId == refId)
            {
                return task.errorMessage;
            }
        }

        return "Task not found";
    }

    uint32_t getMissedExecutions(uint32_t refId)
    {
        for (const Task& task : tasks)
        {
            if (task.refId == refId)
            {
                return task.missedExecutions;
            }
        }

        return 0;
    }

    bool runOnce()
    {
        uint64_t currentTick =
            SuperLoopCommon::getTickTime();

        for (Task& task : tasks)
        {
            uint64_t elapsed =
                currentTick - task.lastRunTick;

            if (elapsed >= task.periodTicks)
            {
                uint64_t executions =
                    elapsed / task.periodTicks;

                if (executions > 1)
                {
                    task.missedExecutions +=
                        static_cast<uint32_t>(executions - 1);
                }

                task.status =
                    SuperLoopCommon::TaskStatus::READY;

                task.status =
                    SuperLoopCommon::TaskStatus::RUNNING;

                try
                {
                    task.workflow();
                }
                catch (const std::exception& e)
                {
                    task.errorCode =
                        SuperLoopCommon::TaskError::TASK_EXCEPTION;

                    task.errorMessage = e.what();

                    task.status = SuperLoopCommon::TaskStatus::ERROR;
                }
                catch (...)
                {
                    task.errorCode =
                        SuperLoopCommon::TaskError::TASK_EXCEPTION;

                    task.errorMessage = "Unknown exception";

                    task.status = SuperLoopCommon::TaskStatus::ERROR;
                }

                task.lastRunTick = currentTick;

                if (task.status != SuperLoopCommon::TaskStatus::ERROR)
                {
                    task.status =
                        SuperLoopCommon::TaskStatus::REGISTERED;
                }
            }
        }

        return true;
    }


    bool startLoop()
    {
        while (true)
        {
            uint64_t currentTick =
                SuperLoopCommon::getTickTime();

            for (Task& task : tasks)
            {
                uint64_t elapsed =
                    currentTick - task.lastRunTick;

                if (elapsed >= task.periodTicks)
                {
                    uint64_t executions =
                        elapsed / task.periodTicks;

                    if (executions > 1)
                    {
                        task.missedExecutions +=
                            static_cast<uint32_t>(executions - 1);
                    }

                    task.status =
                        SuperLoopCommon::TaskStatus::READY;

                    task.status =
                        SuperLoopCommon::TaskStatus::RUNNING;

                    try
                    {
                        task.workflow();

                        task.lastRunTick =
                            currentTick;

                        task.status =
                            SuperLoopCommon::TaskStatus::REGISTERED;
                    }
                    catch (const std::exception& e)
                    {
                        task.status =
                            SuperLoopCommon::TaskStatus::ERROR;

                        task.errorCode =
                            SuperLoopCommon::TaskError::TASK_EXCEPTION;

                        task.errorMessage = e.what();
                    }
                    catch (...)
                    {
                        task.status =
                            SuperLoopCommon::TaskStatus::ERROR;

                        task.errorCode =
                            SuperLoopCommon::TaskError::TASK_EXCEPTION;

                        task.errorMessage =
                            "Unknown exception";
                    }
                }
            }
        }

        return true;
    }

}


void SuperLoop::resetScheduler()
{
    tasks.clear();
    nextRefId = 1;
}




