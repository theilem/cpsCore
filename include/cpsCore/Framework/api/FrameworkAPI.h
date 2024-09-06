/*
 * FrameworkAPI.h
 *
 *  Created on: Feb 1, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_FRAMEWORK_API_FRAMEWORKAPI_H_
#define UAVAP_CORE_FRAMEWORK_API_FRAMEWORKAPI_H_
#include "cpsCore/Aggregation/Aggregator.h"
#include <mutex>

class FrameworkAPI
{
public:

	~FrameworkAPI() = default;

	static FrameworkAPI*
	instance();

	static std::unique_lock<std::mutex>
	lockAggregator();

	static Aggregator*
	getAggregator();

private:

	FrameworkAPI() = default;

	class CGuard
	{
	public:
		~CGuard();
	};

	static FrameworkAPI* instance_;

	std::mutex aggMutex_;
	Aggregator agg_;
};


#endif /* UAVAP_CORE_FRAMEWORK_API_FRAMEWORKAPI_H_ */
