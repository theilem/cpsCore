//
// Created by mirco on 11.12.19.
//

#include "cpsCore/Utilities/Scheduler/ExternalSimulator.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"
#include "cpsCore/Utilities/IPC/IPC.h"
#include "cpsCore/Utilities/IDC/IDC.h"

bool
ExternalSimulator::run(RunStage stage)
{

	switch (stage)
	{

		case RunStage::INIT:
		{
			if (!checkIsSet<MicroSimulator>())
			{
				CPSLOG_ERROR << "MicroSimulator is missing.";
				return true;
			}

			if (!isSet<IPC>())
			{
				if (!checkIsSet<IDC>())
				{
					CPSLOG_ERROR << "IPC and IDC missing. External simulator needs one";
					return true;
				}
			}

			break;
		}
		case RunStage::NORMAL:
		{
			if (isSet<IPC>())
			{
				auto ipc = get<IPC>();

				ipc->subscribe<uint64_t>("sim_time",
										 std::bind(&ExternalSimulator::onSimulateCommand, this, std::placeholders::_1));
			}
			else
			{
				CPSLOG_ERROR << "IDC not implemented yet";
			}
			break;
		}

		case RunStage::FINAL:
			break;
		default:
			break;
	}
	return false;
}

void
ExternalSimulator::onSimulateCommand(const uint64_t& simTime)
{
	auto sim = get<MicroSimulator>();
	if (!sim)
	{
		CPSLOG_ERROR << "Sim missing";
	}

	sim->simulate(Milliseconds(simTime));
}
