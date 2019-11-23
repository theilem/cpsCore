/**
 * @file PluginManager.h
 * @date Sep 20, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */

#ifndef UAVAP_CORE_FRAMEWORK_PLUGINMANAGER_H_
#define UAVAP_CORE_FRAMEWORK_PLUGINMANAGER_H_

#include "cpsCore/Configuration/Configuration.hpp"

class PluginManager
{
public:

	PluginManager() = default;

	bool
	configure(const Configuration& config);

	using PluginHandle = void*;
	using FunctionPtr = void (*)();

	bool
	addPlugin(const std::string& path);

private:

	PluginHandle
	loadPlugin(const std::string& path);

	bool
	registerCreators(PluginHandle handle);
};

#endif /* UAVAP_CORE_FRAMEWORK_PLUGINMANAGER_H_ */
