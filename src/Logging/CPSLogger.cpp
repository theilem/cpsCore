/*
 * CPSLogger.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: mircot
 */

#include <iostream>
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/TimeProvider/ITimeProvider.h"

CPSLogger* CPSLogger::instance_ = nullptr;

CPSLogger*
CPSLogger::instance()
{
	static CGuard g;
	if (!instance_)
		instance_ = new CPSLogger;
	return instance_;
}

void
CPSLogger::setLogLevel(LogLevel level)
{
	setLevel_ = level;
}

void
CPSLogger::setSink(std::ostream& sink)
{
	sink_.rdbuf(sink.rdbuf());
}

std::ostream&
CPSLogger::log(LogLevel level)
{
	if (level >= setLevel_)
	{
		flush();
		isFlushed_ = false; //There will be some input
		if (!moduleName_.empty())
			sink_ << "[" << moduleName_ << "]";
		if (auto tp = timeProvider_.lock())
			sink_ << "[" << Clock::to_time_t(tp->now()) << "]";
		return sink_;
	}
	return emptySink_;
}

std::ostream&
CPSLogger::log(LogLevel level, const std::string& module)
{
	if (moduleName_ == module)
		return log(level);
	return emptySink_;
}

CPSLogger::CPSLogger() :
		setLevel_(LogLevel::WARN), sink_(nullptr), emptySink_(nullptr)
{
	sink_.rdbuf(std::cout.rdbuf());
}

CPSLogger::CGuard::~CGuard()
{
	if (CPSLogger::instance_ != nullptr)
	{
		delete CPSLogger::instance_;
		CPSLogger::instance_ = nullptr;
	}
}

void
CPSLogger::setModuleName(const std::string& name)
{
	moduleName_ = name;
}

void
CPSLogger::flush()
{
	if (isFlushed_)
		return;
	sink_ << std::endl;
	isFlushed_ = true;
}

LogLevel
CPSLogger::getLogLevel() const
{
	return setLevel_;
}

void
CPSLogger::setTimeProvider(std::shared_ptr<ITimeProvider> timeProvider)
{
	timeProvider_ = timeProvider;
}

CPSLogger::LogLevelScope::LogLevelScope(LogLevel setLevel)
{
	oldLogLevel_ = CPSLogger::instance()->getLogLevel();
	CPSLogger::instance()->setLogLevel(setLevel);
}

CPSLogger::LogLevelScope::~LogLevelScope()
{
	CPSLogger::instance()->setLogLevel(oldLogLevel_);
}
