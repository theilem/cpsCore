/*
 * ConfigurableObject.hpp
 *
 *  Created on: Jun 24, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_CONFIGURABLEOBJECT_HPP_
#define UAVAP_CORE_PROPERTYMAPPER_CONFIGURABLEOBJECT_HPP_
#include "cpsCore/Configuration/JsonPopulator.h"
#include "cpsCore/Configuration/Configuration.hpp"
#include "cpsCore/Configuration/PropertyMapper.hpp"


template<class ParameterSet>
class ConfigurableObject: public IConfigurableObject
{
public:

	using ParamType = ParameterSet;

	ConfigurableObject() = default;

	~ConfigurableObject() override = default;

	explicit ConfigurableObject(const ParameterSet& p) :
			params(p)
	{
	}

	void
	setParams(const ParameterSet& set)
	{
		params = set;
	}

	bool
	configure(const Configuration& config) override
	{
		PropertyMapper pm(config);

		params.configure(pm);
		return pm.map();
	}

	void
	parse(Configuration& config) override
	{
		JsonPopulator parser;
		params.configure(parser);
		config.merge_patch(parser.getConfig());
	}

	inline const ParameterSet&
	getParams() const
	{
		return params;
	}

	inline ParameterSet&
	getParams()
	{
		return params;
	}

	template<typename Config>
	inline static ParameterSet
	configureParamsStatic(Config& config)
	{
		ParameterSet p;
		p.configure(config);
		return p;
	}

protected:

	ParameterSet params;
};


#endif /* UAVAP_CORE_PROPERTYMAPPER_CONFIGURABLEOBJECT_HPP_ */
