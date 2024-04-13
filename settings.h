#pragma once

#include "json.h"

#include <unordered_map>

class Settings
{
public:

    static Settings& Instance();

    void set(const std::string& key, float value);

    float get(const std::string& key);

    auto begin()
    {
        return _data.children().begin();
    }

    auto end()
    {
        return _data.children().end();
    }

private:

    std::unordered_map<std::string, float> _cachedFloats;

    json::Json _data{json::Object};
};