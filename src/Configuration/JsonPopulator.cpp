/*
 * JsonPopulator.cpp
 *
 *  Created on: Jul 26, 2019
 *      Author: mirco
 */
#include "cpsCore/Configuration/JsonPopulator.h"
#include <sstream>


std::string
JsonPopulator::getString() const
{
    std::stringstream ss;
    ss << json_.dump(4);
    return ss.str();
}

void
JsonPopulator::toFile(const std::string& path) const
{
    std::fstream file(path, std::ios::out);
    if (!file.is_open())
    {
        CPSLOG_ERROR << "Could not open file " << path;
        return;
    }
    file << json_.dump(4);
}
