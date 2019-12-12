//
// Created by mirco on 11.12.19.
//

#ifndef CPSCORE_EXTERNALSIMULATOR_H
#define CPSCORE_EXTERNALSIMULATOR_H

#include "cpsCore/cps_object"
#include "cpsCore/Utilities/TimeProvider/ITimeProvider.h"
#include "cpsCore/Utilities/Scheduler/IScheduler.h"


class IPC;
class IDC;
class MicroSimulator;


class ExternalSimulator
		: public AggregatableObject<IDC, IPC, MicroSimulator>, public IRunnableObject
{
public:

	static constexpr TypeId typeId = "external_sim";

	bool
	run(RunStage stage) override;

private:

	void
	onSimulateCommand(const uint64_t& simTime);

};


#endif //CPSCORE_EXTERNALSIMULATOR_H
