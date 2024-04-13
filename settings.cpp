#include "settings.h"

#include <stdexcept>

Settings& Settings::Instance()
{
    static Settings settings;
    static bool init{true};
    if(init)
    {
        settings.set("curveFactor", -0.04f);
        settings.set("nLightVfx", 0.0f);
        init = false;
    }
    return settings;
}

void Settings::set(const std::string& key, float value)
{
    _cachedFloats[key] = value;
    if(_data.contains(key))
    {
        _data[key].setValue(std::to_string(value));
    }
    else
    {
        json::Json obj{json::Object, key, std::to_string(value)};
        _data.add(key, obj);
    }
}

float Settings::get(const std::string& key)
{
    auto it = _cachedFloats.find(key);
    if(it != _cachedFloats.end())
    {
        return it->second;
    }
    if(!_data.contains(key))
    {
        throw std::runtime_error("No such setting: " + key);
    }
    return _data[key].get<float>();
};