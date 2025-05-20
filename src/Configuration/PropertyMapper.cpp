//
// Created by Mirco Theile on 19/5/25.
//

#include "cpsCore/Configuration/PropertyMapper.hpp"

bool
PropertyMapper::add(const std::string& key, Duration& val, bool mandatory)
{
    if (auto value = p_.find(key); value != p_.end())
    {
        val = Milliseconds(value->get<int>());
        CPSLOG_TRACE << "Property " << key << " = " << val.count() << " ms";
        return true;
    }
    mandatoryCheck(key, mandatory);
    return false;
}

bool
PropertyMapper::add(const std::string& key, std::string& val, bool mandatory)
{
    if (auto it = p_.find(key); it != p_.end() && it->is_string())
    {
        val = it->get<std::string>();
        CPSLOG_TRACE << "Property " << key << " = " << val;
        return true;
    }
    mandatoryCheck(key, mandatory);
    return false;
}

bool
PropertyMapper::add(const std::string& key, Vector3& val, bool mandatory)
{
    std::vector<FloatingType> vec;
    if (!this->addVector(key, vec, mandatory))
        return false;

    if (vec.size() == 3)
    {
        val = Vector3(vec[0], vec[1], vec[2]);
        return true;
    }
    mandatoryCheck(key, mandatory);
    return false;
}



bool
PropertyMapper::add(const std::string& key, Vector2& val, bool mandatory)
{
    std::vector<FloatingType> vec;
    if (!this->addVector(key, vec, mandatory))
        return false;

    if (vec.size() == 2)
    {
        val = Vector2(vec[0], vec[1]);
        return true;
    }
    mandatoryCheck(key, mandatory);
    return false;
}

bool
PropertyMapper::add(const std::string& key, Configuration& val, bool mandatory)
{
    if (auto it = p_.find(key); it != p_.end() && it->is_object())
    {
        val = *it;
        CPSLOG_TRACE << "Property " << key << " = " << val;
        return true;
    }
    mandatoryCheck(key, mandatory);
    return false;
}
