/*
 * APLoggerTest.cpp
 *
 *  Created on: Jun 24, 2017
 *      Author: mircot
 */

#include "TestInfo.h"

#include "cpsCore/Logging/CPSLogger.h"

#include <sstream>
#include <iostream>


TEST_CASE("Logging Test")
{
	std::ostream sink(nullptr);
	std::ostringstream stream;

	sink.rdbuf(stream.rdbuf());
	CPSLogger::instance()->setSink(sink);
	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
	CPSLogger::instance()->flush();
	stream.str("");

	CPSLOG_ERROR << "error";
	CPSLOG_WARN << "warn";
	CPSLOG_DEBUG << "debug";
	CPSLOG_TRACE << "trace";
	CHECK(stream.str() == "[ERROR] error\n[WARNING] warn\n[DEBUG] debug");

	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	CPSLogger::instance()->flush();
	stream.str("");

	CPSLOG_ERROR << "error";
	CPSLOG_WARN << "warn";
	CPSLOG_DEBUG << "debug";
	CPSLOG_TRACE << "trace";
	CHECK(stream.str() == "");

	CPSLogger::instance()->setLogLevel(LogLevel::TRACE);
	CPSLogger::instance()->flush();
	stream.str("");

	CPSLOG_ERROR << "error";
	CPSLOG_WARN << "warn";
	CPSLOG_DEBUG << "debug";
	CPSLOG_TRACE << "trace";

	CHECK(stream.str() == "[ERROR] error\n[WARNING] warn\n[DEBUG] debug\n[TRACE] trace");

	CPSLogger::instance()->setSink(std::cout);
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
}

