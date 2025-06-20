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
 * SimpleRunner.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: mircot
 */
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Aggregation/Aggregator.h"
#include "cpsCore/Synchronization/SimpleRunner.h"

#include "cpsCore/Utilities/EnumMap.hpp"

SimpleRunner::SimpleRunner(Aggregator& agg) :
		agg_(agg)
{
}

bool
SimpleRunner::runStage(RunStage stage)
{
	auto runnableObjects = agg_.getAll<IRunnableObject>();
	bool error = false;
	for (auto it : runnableObjects)
	{
		if (it->run(stage))
			error = true;
	}
	return error;
}

bool
SimpleRunner::runStages(const std::vector<RunStage>& stages)
{
	for (auto stage : stages)
	{
		CPSLOG_DEBUG << "Run stage " << EnumMap<RunStage>::convert(stage);
		if (runStage(stage))
		{
			CPSLOG_ERROR << "Run stage " << EnumMap<RunStage>::convert(stage) << " failed";
			return true;
		}
	}
	return false;
}

bool
SimpleRunner::runAllStages()
{
	CPSLOG_DEBUG << "Run stage INIT";
	if (runStage(RunStage::INIT))
	{
		CPSLOG_ERROR << "Errors occured in RunStage INIT";
		return true;
	}
	CPSLOG_DEBUG << "Run stage NORMAL";
	if (runStage(RunStage::NORMAL))
	{
		CPSLOG_ERROR << "Errors occured in RunStage NORMAL";
		return true;
	}
	CPSLOG_DEBUG << "Run stage FINAL";
	if (runStage(RunStage::FINAL))
	{
		CPSLOG_ERROR << "Errors occured in RunStage FINAL";
		return true;
	}
	CPSLOG_DEBUG << "Run stages finished successfull";
	return false;
}
