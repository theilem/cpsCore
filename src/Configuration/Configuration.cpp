//
// Created by mirco on 27.11.19.
//

#include "cpsCore/Configuration/Configuration.hpp"

#include <fstream>
#include <sstream>


Configuration
parseConfigFile(const std::string& path)
{
	Configuration conf;
	if (!path.empty())
	{
		std::ifstream f(path);
		conf = json::parse(f);
	}
	return conf;
}

Configuration
parseConfigString(const std::string& config)
{
	Configuration conf;
	if (!config.empty())
	{
		std::istringstream iss(config);
		conf = json::parse(config);
	}
	return conf;
}
