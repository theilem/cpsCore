/*
 * APLoggerTest.cpp
 *
 *  Created on: Jun 24, 2017
 *      Author: mircot
 */

#include <cpsCore/Utilities/Test/TestInfo.h>

#include <cpsCore/Logging/CPSLogger.h>

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
	auto line1 = __LINE__;
	CPSLOG_WARN << "warn";
	auto line2 = __LINE__;
	CPSLOG_DEBUG << "debug";
	auto line3 = __LINE__;
	CPSLOG_TRACE << "trace";

	std::stringstream result;
	result << " [ERROR] [" << __FILE__ << ":" << line1 - 1 << "] error\n";
	result << " [WARNING] [" << __FILE__ << ":" << line2 - 1 << "] warn\n";
	result << " [DEBUG] [" << __FILE__ << ":" << line3 - 1 << "] debug";
	CHECK(stream.str() == result.str());

	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	CPSLogger::instance()->flush();
	stream.str("");

	CPSLOG_ERROR << "error";
	CPSLOG_WARN << "warn";
	CPSLOG_DEBUG << "debug";
	CPSLOG_TRACE << "trace";
	CHECK(stream.str().empty());

	CPSLogger::instance()->setLogLevel(LogLevel::TRACE);
	CPSLogger::instance()->flush();
	stream.str("");
	result.str("");

	CPSLOG_ERROR << "error";
	line1 = __LINE__;
	CPSLOG_WARN << "warn";
	line2 = __LINE__;
	CPSLOG_DEBUG << "debug";
	line3 = __LINE__;
	CPSLOG_TRACE << "trace";
	auto line4 = __LINE__;

	result << " [ERROR] ["<< __FILE__ << ":" << line1 - 1 << "] error\n";
	result << " [WARNING] ["<< __FILE__ << ":" << line2 - 1 << "] warn\n";
	result << " [DEBUG] ["<< __FILE__ << ":" << line3 - 1 << "] debug\n";
	result << " [TRACE] ["<< __FILE__ << ":" << line4 - 1 << "] trace";
	CHECK(stream.str() == result.str());
	CPSLogger::instance()->flush();

	stream.str("");
	result.str("");

	CPSLOG_ERROR;
	line1 = __LINE__;
	CPSLOG_WARN;
	line2 = __LINE__;
	CPSLOG_DEBUG;
	line3 = __LINE__;
	CPSLOG_TRACE;
	line4 = __LINE__;
	result << " [ERROR] ["<< __FILE__ << ":" << line1 - 1 << "] \n";
	result << " [WARNING] ["<< __FILE__ << ":" << line2 - 1 << "] \n";
	result << " [DEBUG] ["<< __FILE__ << ":" << line3 - 1 << "] \n";
	result << " [TRACE] ["<< __FILE__ << ":" << line4 - 1 << "] ";
	CHECK(stream.str() == result.str());

	CPSLogger::instance()->setSink(std::cout);
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
}

