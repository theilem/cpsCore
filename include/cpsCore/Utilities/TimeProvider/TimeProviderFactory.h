/*
 * TimeProviderFactory.h
 *
 *  Created on: Jul 26, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_TIMEPROVIDER_TIMEPROVIDERFACTORY_H_
#define UAVAP_CORE_TIMEPROVIDER_TIMEPROVIDERFACTORY_H_

#include "cpsCore/Framework/StaticFactory.h"
#include "cpsCore/Utilities/TimeProvider/ITimeProvider.h"
#include "cpsCore/Utilities/TimeProvider/SystemTimeProvider.h"

using TimeProviderFactory = StaticFactory<ITimeProvider, false, SystemTimeProvider>;

#endif /* UAVAP_CORE_TIMEPROVIDER_TIMEPROVIDERFACTORY_H_ */
