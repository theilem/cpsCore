/**
 * @file Time.h
 * @brief Typedefs for usage of time in cpsCore based on std::chrono
 * @date Nov 23rd 2019
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_TIME_H_
#define UAVAP_CORE_TIME_H_

#include <chrono>
#include <ctime>
#include <cpsCore/Utilities/DataPresentation/detail/Split.h>
#include <sstream>

using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = Clock::duration;

enum class TimeUnit
{
    Nanoseconds,
    Microseconds,
    Milliseconds,
    Seconds,
    Minutes,
    Hours
};

template <typename Type>
using TimedValue = std::pair<TimePoint, Type>;


// Enable if is timed value
template <typename Type>
struct is_timed_value : std::false_type
{
};

template <typename Type>
struct is_timed_value<TimedValue<Type>> : std::true_type
{
};

inline std::tm*
getTM(const TimePoint& time)
{
    auto timeT = Clock::to_time_t(time);
    return std::localtime(&timeT);
}

inline Duration
getTimeOfDay(const TimePoint& time)
{
    Hours sinceEpoch = std::chrono::duration_cast<Hours>(time.time_since_epoch());
    return time.time_since_epoch() - Hours((sinceEpoch.count() / 24) * 24);
}

inline long
durationToNanoseconds(const Duration& duration)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

inline long
timePointToNanoseconds(const TimePoint& timePoint)
{
    return durationToNanoseconds(timePoint.time_since_epoch());
}

inline std::string
humanReadableTimeOfDay(const TimePoint& timePoint)
{
    auto tod = std::chrono::duration_cast<Milliseconds>(getTimeOfDay(timePoint));
    auto h = std::chrono::duration_cast<Hours>(tod);
    tod -= h;
    auto m = std::chrono::duration_cast<Minutes>(tod);
    tod -= m;
    auto s = std::chrono::duration_cast<Seconds>(tod);
    tod -= s;
    auto ms = std::chrono::duration_cast<Milliseconds>(tod);
    std::stringstream ss;
    ss << h.count() << "h_" << m.count() << "m_" << s.count() << "s_" << ms.count() << "ms";
    return ss.str();
}

inline std::ostream&
operator<<(std::ostream& os, const TimePoint& t)
{
    os << humanReadableTimeOfDay(t);
    return os;
}

// Add stream operator for timed value to print to std::cout
template <typename Type>
std::ostream&
operator<<(std::ostream& os, const TimedValue<Type>& tv)
{
    os << "Time: " << tv.first << std::endl;
    os << "Value: " << tv.second;
    return os;
}

namespace dp
{
    template <class Archive, typename Type>
    inline void
    store(Archive& ar, const TimePoint& t)
    {
        static_assert(
            std::is_same_v<TimePoint::duration, Nanoseconds> || std::is_same_v<TimePoint::duration, Microseconds> ||
            std::is_same_v<TimePoint::duration, Milliseconds> || std::is_same_v<TimePoint::duration, Seconds> ||
            std::is_same_v<TimePoint::duration, Minutes> || std::is_same_v<TimePoint::duration, Hours>,
            "Unknown time unit");
        if constexpr (std::is_same_v<TimePoint::duration, Nanoseconds>)
            ar << TimeUnit::Nanoseconds;
        else if constexpr (std::is_same_v<TimePoint::duration, Microseconds>)
            ar << TimeUnit::Microseconds;
        else if constexpr (std::is_same_v<TimePoint::duration, Milliseconds>)
            ar << TimeUnit::Milliseconds;
        else if constexpr (std::is_same_v<TimePoint::duration, Seconds>)
            ar << TimeUnit::Seconds;
        else if constexpr (std::is_same_v<TimePoint::duration, Minutes>)
            ar << TimeUnit::Minutes;
        else if constexpr (std::is_same_v<TimePoint::duration, Hours>)
            ar << TimeUnit::Hours;

        ar << t.time_since_epoch().count();
    }

    template <class Archive, typename Type>
    inline void
    load(Archive& ar, TimePoint& t)
    {
        TimeUnit unit{};
        ar >> unit;

        if (unit == TimeUnit::Nanoseconds)
        {
            Nanoseconds::rep rep;
            ar >> rep;
            t = TimePoint() + std::chrono::duration_cast<TimePoint::duration>(Nanoseconds(rep));
        }
        else if (unit == TimeUnit::Microseconds)
        {
            Microseconds::rep rep;
            ar >> rep;
            t = TimePoint() + std::chrono::duration_cast<TimePoint::duration>(Microseconds(rep));
        }
        else if (unit == TimeUnit::Milliseconds)
        {
            Milliseconds::rep rep;
            ar >> rep;
            t = TimePoint() + std::chrono::duration_cast<TimePoint::duration>(Milliseconds(rep));
        }
        else if (unit == TimeUnit::Seconds)
        {
            Seconds::rep rep;
            ar >> rep;
            t = TimePoint() + std::chrono::duration_cast<TimePoint::duration>(Seconds(rep));
        }
        else if (unit == TimeUnit::Minutes)
        {
            Minutes::rep rep;
            ar >> rep;
            t = TimePoint() + std::chrono::duration_cast<TimePoint::duration>(Minutes(rep));
        }
        else if (unit == TimeUnit::Hours)
        {
            Hours::rep rep;
            ar >> rep;
            t = TimePoint() + std::chrono::duration_cast<TimePoint::duration>(Hours(rep));
        }
        else
        {
        }
    }

    template <class Archive, typename Type>
    inline void
    serialize(Archive& ar, TimePoint& t)
    {
        split(ar, t);
    }
}

#endif /* UAVAP_CORE_TIME_H_ */
