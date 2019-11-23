/**
 * @file LockTypes.h
 * @brief Standard lock typedefs
 * @date Jun 27, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_LOCKTYPES_H_
#define UAVAP_CORE_LOCKTYPES_H_

#ifdef NO_LOCKING

using Mutex = int;

struct FakeLock
{
	FakeLock(const Mutex& mut){}

	void
	unlock(){}
};
using Lock = FakeLock;
using LockGuard = int;

#else

#include <mutex>

#if __cplusplus >= 201703L

#include <shared_mutex>

using SharedMutex = std::shared_mutex;
#else

using SharedMutex = std::mutex;
#endif

using Mutex = std::mutex;
using Lock = std::unique_lock<Mutex>;
using LockGuard = std::lock_guard<Mutex>;
#endif

#endif /* UAVAP_CORE_LOCKTYPES_H_ */
