#pragma once
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;

json jsonArrayValueFinder(const json & json_data, const std::string named_array, const std::string field_name, const std::string value);

int GetOffsetFromJson(json& j, std::string name);