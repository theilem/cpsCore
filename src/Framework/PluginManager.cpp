/**
 * @file PluginManager.cpp
 * @date Sep 20, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */
#include <dlfcn.h>

#include "cpsCore/Configuration/PropertyMapper.hpp"
#include "cpsCore/Framework/PluginManager.h"

bool
PluginManager::configure(const Configuration& config)
{
	PropertyMapper pm(config);
	Configuration plugins;
	pm.add("plugins", plugins, true);

	for (const auto& it : plugins)
	{
		auto path = it.second.get_value<std::string>();
		if (!addPlugin(path))
			CPSLOG_ERROR << "Plugin cannot be loaded.";
	}
	return pm.map();
}

bool
PluginManager::addPlugin(const std::string& path)
{
	auto handle = loadPlugin(path);

	if (!handle)
	{
#ifndef NO_DL
		CPSLOG_ERROR << "Cannot load shared object at: " << path << ": " << dlerror();
#endif
		return false;
	}
	CPSLOG_TRACE << "Handle found";

	if (!registerCreators(handle))
	{
		CPSLOG_ERROR << "Cannot register creators for shared object at: " << path;
		return false;
	}
	CPSLOG_TRACE << "Creators registered";

	return true;
}

PluginManager::PluginHandle
PluginManager::loadPlugin(const std::string& path)
{
	CPSLOG_TRACE << "Opening shared object from: " << path;
#ifndef NO_DL
	return dlopen(path.c_str() , RTLD_NOW);
#else
	return nullptr;
#endif
}

bool
PluginManager::registerCreators(PluginHandle handle)
{
#ifndef NO_DL
	void* registerPlugin = dlsym(handle, "register_plugin");
#else
	void* registerPlugin = nullptr;
#endif

	if (registerPlugin == nullptr)
	{
		CPSLOG_ERROR << "Cannot register creators, 'register_plugin' handle not found";
		return false;
	}

	auto func = reinterpret_cast<FunctionPtr>(registerPlugin);
	func();

	return true;

}
