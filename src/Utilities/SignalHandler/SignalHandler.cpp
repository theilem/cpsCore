/*
 * SignalHandler.cpp
 *
 *  Created on: Jul 15, 2019
 *      Author: mirco
 */
#include <cpsCore/Utilities/SignalHandler/SignalHandler.h>
#include "cpsCore/Logging/CPSLogger.h"
#include <uavAP/Core/Scheduler/IScheduler.h>
#include <csignal>
#include <cstdlib>

void
sigIntHandler(int sig)
{
	SignalHandlerSingleton::getInstance().callSigHandlers(sig);
	CPSLogger::instance()->flush(); //Synchronize stdio
}

SignalHandler::SignalHandler(Aggregator& agg) :
		agg_(agg)
{
	CPSLOG_TRACE << "Signal handler created";
	SignalHandlerSingleton::getInstance().subscribeOnExit(std::bind(&SignalHandler::onExit, this));
}

void
SignalHandler::subscribeOnSigint(const OnSIGINT::slot_type& slot)
{
	CPSLOG_TRACE << "SignalHandler: Subscribing on sigint";
	SignalHandlerSingleton::getInstance().subscribeOnSigint(slot);
}

void
SignalHandler::subscribeOnExit(const OnExit::slot_type& slot)
{
	CPSLOG_TRACE << "SignalHandler: Subscribing on exit";
	SignalHandlerSingleton::getInstance().subscribeOnExit(slot);
}
