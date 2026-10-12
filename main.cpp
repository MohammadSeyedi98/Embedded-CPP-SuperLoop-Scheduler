#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstdint>
#include <thread>
#include <chrono>

#include "../src/SuperLoopConfig.hpp"
#include "../src/SuperLoopCommon.hpp"
#include "../src/SuperLoop.hpp"



bool testScheduler()
{

    int count1 = 0, count2 = 0, count3 = 0, count4 = 0;

    SuperLoop::registerTask([&count1]() { count1++; }, 100);
    SuperLoop::registerTask([&count2]() { count2++; }, 200);
    SuperLoop::registerTask([&count3]() { count3++; }, 500);
    SuperLoop::registerTask([&count4]() { count4++; }, 1000);

    for (int i = 0; i < 100; i++)
    {
        SuperLoopCommon::IRQHandler();
        SuperLoop::runOnce();
    }

    std::cout << "\n--- Scheduler test ---\n";
    std::cout << "Task 1: expected 10, actual " << count1 << '\n';
    std::cout << "Task 2: expected 5, actual " << count2 << '\n';
    std::cout << "Task 3: expected 2, actual " << count3 << '\n';
    std::cout << "Task 4: expected 1, actual " << count4 << '\n';

    bool passed = count1 == 10 && count2 == 5 && count3 == 2 && count4 == 1;

    SuperLoop::resetScheduler();

    return passed;
}

bool testMissedExecutions()
{
    int count = 0;

    uint32_t refId = SuperLoop::registerTask(
        [&count]() { count++; },
        100
    );

    for (int i = 0; i < 50; i++)
    {
        SuperLoopCommon::increaseTickTime();
    }

    SuperLoop::runOnce();

    uint32_t missed = SuperLoop::getMissedExecutions(refId);

    std::cout << "\n--- Missed executions test ---\n";
    std::cout << "Actual executions: " << count << '\n';
    std::cout << "Missed executions: " << missed << '\n';

    bool passed = (refId != 0) &&
                  (count == 1) &&
                  (missed == 4);

    std::cout << "Missed executions test: "
              << (passed ? "PASSED" : "FAILED") << '\n';

    SuperLoop::resetScheduler();

    return passed;
}

bool testTaskError()
{
    uint32_t refId = SuperLoop::registerTask(
        []()
        {
            throw std::runtime_error("Test error");
        },
        100
    );

    for (int i = 0; i < 10; i++)
    {
        SuperLoopCommon::increaseTickTime();
    }

    SuperLoop::runOnce();

    std::cout << "Error name: "
              << SuperLoopCommon::taskErrorToString(
                     SuperLoopCommon::TaskError::TASK_EXCEPTION)
              << '\n';

    std::cout << "Error message: "
              << SuperLoop::getTaskErrorMessage(refId)
              << '\n';

    std::cout << "\n--- Task error test ---\n";
    std::cout << "Task status: "
              << static_cast<int>(SuperLoop::getTaskStatus(refId))
              << '\n';

    std::cout << "Error code: "
              << SuperLoop::getTaskErrorCode(refId) << '\n';

    std::cout << "Error message: "
              << SuperLoop::getTaskErrorMessage(refId) << '\n';

    std::cout << "Invalid task status: "
              << static_cast<int>(SuperLoop::getTaskStatus(999)) << '\n';

    std::cout << "Invalid task error code: "
              << SuperLoop::getTaskErrorCode(999) << '\n';

    std::cout << "Invalid task error message: "
              << SuperLoop::getTaskErrorMessage(999) << '\n';

    std::cout << "Invalid task missed executions: "
              << SuperLoop::getMissedExecutions(999) << '\n';

    bool passed = refId != 0 &&
                  SuperLoop::getTaskErrorCode(refId) != 0;

    SuperLoop::resetScheduler();

    return passed;
}

bool testTaskCapacity()
{
    std::cout << "\n--- Task capacity test ---\n";

    uint32_t refId = 0;

    for (int i = 1; i <= 11; i++)
    {
        refId = SuperLoop::registerTask(
            []() {},
            100
        );

        std::cout << "Task " << i
                  << " refId: " << refId
                  << '\n';
    }

    // با ظرفیت پیش‌فرض 10، ثبت تسک یازدهم باید رد شود.
    bool passed = (refId == 0);

    SuperLoop::resetScheduler();

    return passed;
}

bool testCRC()
{
    const uint8_t data[] = {
        0x01, 0x03, 0x00, 0x00, 0x00, 0x0A
    };

    const size_t length = sizeof(data);

    uint16_t crc =
        SuperLoopCommon::calculateCRC16(data, length);

    bool valid =
        SuperLoopCommon::validateCRC16(data, length, crc);

    uint8_t corruptedData[] = {
        0x01, 0x03, 0x00, 0x01, 0x00, 0x0A
    };

    bool invalid =
        SuperLoopCommon::validateCRC16(
            corruptedData, length, crc
        );

    std::cout << "\n--- CRC test ---\n";
    std::cout << std::boolalpha;
    std::cout << "CRC: 0x" << std::hex << crc << std::dec << '\n';
    std::cout << "Valid data: " << valid << '\n';
    std::cout << "Corrupted data: " << invalid << '\n';

    return valid && !invalid;
}

void testIRQThread()
{
    uint64_t before = SuperLoopCommon::getTickTime();

    std::thread irqThread([]()
    {
        for (int i = 0; i < 100; i++)
        {
            SuperLoopCommon::IRQHandler();

            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );
        }
    });

    irqThread.join();

    uint64_t after = SuperLoopCommon::getTickTime();

    uint64_t generatedTicks = after - before;

    std::cout << "\n--- IRQ Thread test ---\n";
    std::cout << "Ticks generated: "
              << generatedTicks << '\n';

    if (generatedTicks == 100)
        std::cout << "IRQ Thread test: PASSED\n";
    else
        std::cout << "IRQ Thread test: FAILED\n";
}

void testSchedulerThread()
{
    SuperLoop::resetScheduler();

    int counter = 0;

    SuperLoop::registerTask([&counter]()
    {
        counter++;
    }, 10);

    uint64_t before = SuperLoopCommon::getTickTime();

    std::thread irqThread([]()
    {
        for (int i = 0; i < 100; i++)
        {
            SuperLoopCommon::IRQHandler();
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1));
        }
    });

    for (int i = 0; i < 120; i++)
    {
        SuperLoop::runOnce();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }

    irqThread.join();

    std::cout << "\n--- Scheduler Thread test ---\n";
    std::cout << "Ticks generated: "
              << SuperLoopCommon::getTickTime() - before << '\n';
    std::cout << "Task executions: " << counter << '\n';
    std::cout << "Scheduler Thread test: "
              << (counter > 0 ? "PASSED" : "FAILED") << '\n';

    SuperLoop::resetScheduler();
}

int main()
{
    if (!SuperLoopConfig::configPerSecond(100))
    {
        std::cerr << "Failed to configure tick rate\n";
        return 1;
    }

    bool schedulerPassed = testScheduler();
    bool missedPassed = testMissedExecutions();
    bool errorPassed = testTaskError();
    bool capacityPassed = testTaskCapacity();
    bool crcPassed = testCRC();


    bool allPassed = schedulerPassed && missedPassed && errorPassed && capacityPassed && crcPassed;

    testIRQThread();
    testSchedulerThread();

    std::cout << "\n-------------------------\n";
    std::cout << "Overall result: "
              << (allPassed ? "PASSED" : "FAILED") << '\n';

    return allPassed ? 0 : 1;
}
