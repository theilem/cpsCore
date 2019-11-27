//
// Created by mirco on 27.11.19.
//

#include <boost/property_tree/json_parser.hpp>
#include "cpsCore/Configuration/Configuration.hpp"

Configuration
parseConfigFile(const std::string& path)
{
	Configuration conf;
	if (!path.empty())
	{
		boost::property_tree::read_json(path, conf);
	}
	return conf;
}