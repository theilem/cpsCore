//
// Created by mirco on 17.11.19.
//

#ifndef UAVAP_MICROSIMULATORPARAMS_H
#define UAVAP_MICROSIMULATORPARAMS_H

#include "cpsCore/Configuration/Parameter.hpp"

struct MicroSimulatorParams
{
	Parameter<float> realTimeFactor = {0, "real_time_factor", false};
	Parameter<unsigned> simTime = {100, "sim_time_s", false};
	Parameter<bool> showProgress = {false, "show_progress", false};

	template<typename Config>
	void
	configure(Config& c)
	{
		c & realTimeFactor;
		c & simTime;
		c & showProgress;
	}
};


#endif //UAVAP_MICROSIMULATORPARAMS_H
