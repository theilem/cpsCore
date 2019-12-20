/*
 * SchedulerFactory.h
 *
 *  Created on: Jul 26, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_SCHEDULER_SCHEDULERFACTORY_H_
#define UAVAP_CORE_SCHEDULER_SCHEDULERFACTORY_H_

#include "cpsCore/Framework/StaticFactory.h"
#include "cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"

using SchedulerFactory = StaticFactory<IScheduler, false, MultiThreadingScheduler, MicroSimulator>;

#endif /* UAVAP_CORE_SCHEDULER_SCHEDULERFACTORY_H_ */
