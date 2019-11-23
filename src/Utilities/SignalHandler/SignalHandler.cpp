/*
 * SignalHandler.cpp
 *
 *  Created on: Jul 15, 2019
 *      Author: mirco
 */
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"
#include "cpsCore/Logging/CPSLogger.h"

void
sigIntHandler(int sig)
{
	SignalHandlerSingleton::getInstance().callSigHandlers(sig);
	CPSLogger::instance()->flush(); //Synchronize stdio
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
