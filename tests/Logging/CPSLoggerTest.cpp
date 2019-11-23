////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 University of Illinois Board of Trustees
//
// This file is part of uavAP.
//
// uavAP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// uavAP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////
/*
 * APLoggerTest.cpp
 *
 *  Created on: Jun 24, 2017
 *      Author: mircot
 */

#include <catch2/catch.hpp>

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

	CPSLOG_ERROR << "error";
	CPSLOG_WARN << "warn";
	CPSLOG_DEBUG << "debug";
	CPSLOG_TRACE << "trace";
	CHECK(stream.str().compare("[ERROR] error\n[WARNING] warn\n[DEBUG] debug\n") == 0);
	stream.str("");

	CPSLogger::instance()->setLogLevel(LogLevel::NONE);

	CPSLOG_ERROR << "error";
	CPSLOG_WARN << "warn";
	CPSLOG_DEBUG << "debug";
	CPSLOG_TRACE << "trace";
	CHECK(stream.str().compare("") == 0);
	CPSLogger::instance()->flush();
	stream.str("");

	CPSLogger::instance()->setLogLevel(LogLevel::TRACE);

	CPSLOG_ERROR << "error";
	CPSLOG_WARN << "warn";
	CPSLOG_DEBUG << "debug";
	CPSLOG_TRACE << "trace";

	CHECK(stream.str().compare("[ERROR] error\n[WARNING] warn\n[DEBUG] debug\n[TRACE] trace") == 0);
	stream.str("");

	CPSLogger::instance()->setSink(std::cout);
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
}

