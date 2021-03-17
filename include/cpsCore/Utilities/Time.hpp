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

//using Clock = std::chrono::high_resolution_clock;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = Clock::duration;
using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;

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
	auto tod = std::chrono::duration_cast<Nanoseconds>(getTimeOfDay(timePoint));
	auto h = std::chrono::duration_cast<Hours>(tod);
	tod -= h;
	auto m = std::chrono::duration_cast<Minutes>(tod);
	tod -= m;
	auto s = std::chrono::duration_cast<Seconds>(tod);
	tod -= s;
	auto ms = std::chrono::duration_cast<Milliseconds>(tod);
	std::stringstream ss;
	ss << h.count() << "h:" << m.count() << "m:" << s.count() << "s:" << ms.count() << "ms";
	return ss.str();
}

namespace dp
{

template<class Archive, typename Type>
inline void
store(Archive& ar, const TimePoint& t)
{
	ar << t.time_since_epoch().count();
}

template<class Archive, typename Type>
inline void
load(Archive& ar, TimePoint& t)
{
	TimePoint::rep rep;
	ar >> rep;

	TimePoint tNew;
	tNew += TimePoint::duration(rep);
	t = tNew;
}

template<class Archive, typename Type>
inline void
serialize(Archive& ar, TimePoint& t)
{
	split(ar, t);
}
}

#endif /* UAVAP_CORE_TIME_H_ */
