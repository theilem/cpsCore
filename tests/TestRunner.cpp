/**
 * @file TestRunner.cpp
 * @date Oct 2, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "TestInfo.h"

int
main(int argc, char** argv)
{
	TestInfo::getInstance().initialize(argc, argv);

	return Catch::Session().run(argc, argv);
}
