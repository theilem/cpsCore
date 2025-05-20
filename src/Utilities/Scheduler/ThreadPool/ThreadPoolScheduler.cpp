//
// Created by Mirco Theile on 17/5/25.
//

#include "cpsCore/Utilities/Scheduler/ThreadPool/ThreadPoolScheduler.h"

#include "cpsCore/Utilities/Scheduler/ThreadPool/Task.h"
#include "cpsCore/Utilities/TimeProvider/ITimeProvider.h"

void
ThreadPoolScheduler::threadLoop()
{
    auto timeProv = get<ITimeProvider>();
    while (!canceled_.load())
    {
        std::unique_lock lock(tasksMutex_);
        auto now = timeProv->now();
        auto itToRun = tasks_.end();
        for (auto it = tasks_.begin(); it != tasks_.end(); ++it)
        {
            const auto& time = it->first;
            const auto& task = it->second;
            if (time > now)
            {
                timeProv->waitUntil(it->first, taskReleased_, lock);
                break;
            }
            if (task->running)
                continue;
            if (!task->depends.empty() || !task->provides.empty())
            {
                // Task depends on other tasks. Check if any of them is running or is in the tasks queue
                // with equal or less time
                bool dependencyFound = false;
                for (const auto& [t, other]: tasks_)
                {
                    if (t > time)
                        // No task with equal or less time found
                        break;
                    // Check if any of the tasks with higher or equal time prio provides a dependency
                    for (const auto& depends : task->depends)
                    {
                        if (other->provides.count(depends) > 0)
                        {
                            dependencyFound = true;
                            break;
                        }
                    }
                    if (dependencyFound)
                        break;
                    if (t == time)
                        // Do not check provide collision with equal time
                        break;
                    // Check if any of the tasks with higher time prio depends on a providing
                    for (const auto& provide : task->provides)
                    {
                        if (other->depends.count(provide) > 0)
                        {
                            dependencyFound = true;
                            break;
                        }
                    }
                    if (dependencyFound)
                        break;
                }
                if (dependencyFound)
                    // Task cannot be run yet
                    continue;
            }
            // Task is ready to be run and has no dependency. Run it now using this thread.
            itToRun = it;
            break;
        }
        if (itToRun != tasks_.end())
        {
            itToRun->second->running = true;
            lock.unlock();
            itToRun->second->task();
            lock.lock();
            itToRun->second->running = false;
            itToRun->second->finished = true;

        }
    }
}
