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
 * IRunnableObject.h
 *
 *  Created on: Jun 29, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_OBJECT_IRUNNABLEOBJECT_H_
#define UAVAP_CORE_OBJECT_IRUNNABLEOBJECT_H_

#include "cpsCore/Utilities/EnumMap.hpp"

enum class RunStage
{
	INIT, NORMAL, FINAL, SYNCHRONIZE
};

ENUMMAP_INIT(RunStage, {{RunStage::INIT, "init"}, {RunStage::NORMAL, "normal"},
		{RunStage::FINAL, "final"}, {RunStage::SYNCHRONIZE, "synchronize"}});

class IRunnableObject
{

public:

	virtual
	~IRunnableObject() = default;

	//! Returns true on error
	virtual bool
	run(RunStage stage) = 0;

	bool
	runAllStages(){
		if (run(RunStage::INIT))
			return true;
		if (run(RunStage::NORMAL))
			return true;
		return run(RunStage::FINAL);
	}

};

#endif /* UAVAP_CORE_OBJECT_IRUNNABLEOBJECT_H_ */
