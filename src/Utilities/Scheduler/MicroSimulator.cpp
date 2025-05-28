/*
 * MicroSimulator.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: mircot
 */

#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"
#include "cpsCore/Utilities/Scheduler/EventBody.h"
#include <iostream>
#include <utility>
#include <iostream>

MicroSimulator::MicroSimulator() :
    now_(), runs_(0), stopOnWait_(false), waitCondition_(
        nullptr), waitReleased_(false), stopped_(false)
{
}

bool
MicroSimulator::run(RunStage stage)
{
    switch (stage)
    {
    case RunStage::INIT:
        {
            realTimeFactor_ = params.realTimeFactor();
            break;
        }
    default:
        break;
    }
    return false;
}

MicroSimulator::~MicroSimulator()
{
}

Event
MicroSimulator::schedule(const std::function<void
                             ()>& task, Duration initialFromNow, const std::string& eventName)
{
    auto body = std::make_shared<EventBody>(task, eventName);
    events_.insert(std::make_pair(now_ + initialFromNow, body));
    return Event(body);
}

Event
MicroSimulator::schedule(const std::function<void
                             ()>& task, Duration initialFromNow, Duration period, const std::string& eventName)
{
    auto body = std::make_shared<EventBody>(task, period, eventName);
    events_.insert(std::make_pair(now_ + initialFromNow, body));
    return Event(body);
}

void
MicroSimulator::stop()
{
    CPSLOG_DEBUG << "Stop MicroSim called";
    stopped_ = true;
}

int
MicroSimulator::simulate(Duration duration)
{
    runs_ = 0;
    auto startTimeReal = std::chrono::high_resolution_clock::now();
    auto startTimeSim = now_;

    TimePoint endSim = now_ + duration;
    if (params.showProgress())
    {
        schedule([this, endSim, duration]() { logProgress(endSim, duration); }, duration / 100, duration / 100,
                 "progress");
    }
    while (now_ <= endSim)
    {
        std::unique_lock lock(simMutex_);
        if (events_.empty())
        {
            break;
        }

        if (events_.begin()->first > endSim)
        {
            break;
        }

        if (realTimeFactor_ > 0)
        {
            lock.unlock();
            std::this_thread::sleep_until(startTimeReal + (events_.begin()->first - startTimeSim) * realTimeFactor_);
            lock.lock();
        }

        if (stopped_)
        {
            CPSLOG_DEBUG << "MicroSim Scheduler was stopped.";
            return runs_;
        }
        auto [time, nextEvent] = *events_.begin();
        now_ = time;
        if (!nextEvent->isCanceled.load())
        {
            CPSLOG_TRACE << "Calling function at " << Clock::to_time_t(now_);
            nextEvent->body();
            if (nextEvent->period)
            {
                events_.insert(std::make_pair(now_ + *nextEvent->period, nextEvent));
            }
            ++runs_;
        }
        events_.erase(events_.begin());
    }

    now_ = endSim;
    return runs_;
}

void
MicroSimulator::stopOnWait()
{
    stopOnWait_ = true;
}

void
MicroSimulator::setMainThread()
{
}

void
MicroSimulator::startSchedule()
{
}

void
MicroSimulator::releaseWait()
{
    std::unique_lock<std::mutex> lock(waitCondMutex_);
    if (!waitCondition_)
    {
        CPSLOG_WARN << "Nobody waiting on release";
        return;
    }
    waitReleased_ = true;
    waitCondition_->notify_all();
}

TimePoint
MicroSimulator::now()
{
    return now_;
}

bool
MicroSimulator::waitFor(Duration duration, std::condition_variable& interrupt,
                        std::unique_lock<std::mutex>& lock)
{
    if (stopOnWait_)
    {
        waitCondition_ = &interrupt;
        waitCondition_->wait(lock);
    }
    bool waitReleased = waitReleased_;
    waitReleased_ = false;
    if (waitReleased)
        now_ += duration;
    return waitReleased;
}

bool
MicroSimulator::waitUntil(TimePoint timePoint, std::condition_variable& interrupt,
                          std::unique_lock<std::mutex>& lock)
{
    if (stopOnWait_)
    {
        waitCondition_ = &interrupt;
        waitCondition_->wait(lock);
    }
    bool waitReleased = waitReleased_;
    waitReleased_ = false;
    if (waitReleased)
        now_ = timePoint;
    return waitReleased;
}

void
MicroSimulator::clearSchedule()
{
    events_.clear();
}

bool
MicroSimulator::isStopped()
{
    return stopped_;
}

std::mutex&
MicroSimulator::getSimMutex()
{
    return simMutex_;
}

void
MicroSimulator::setRealTimeFactor(float factor)
{
    realTimeFactor_ = factor;
}

int
MicroSimulator::simulate()
{
    return simulate(Seconds(params.simTime()));
}

Duration
MicroSimulator::timeSinceStart()
{
    return now() - TimePoint();
}

void
MicroSimulator::drawProgressBar(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

void
MicroSimulator::logProgress(const TimePoint point, const Duration duration)
{
    drawProgressBar(1 - (point - now_).count() / float(duration.count()));
}
