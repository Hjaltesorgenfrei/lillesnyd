#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>

#include "jsonutil.h"

// for convenience



//Only works if there is one 
json jsonArrayValueFinder(const json & json_data, const std::string named_array, const std::string field_name, const std::string value) {
	for (auto i : json_data[named_array]) {
		if (i[field_name] == value) {
			return i;
		}
	}

	throw std::runtime_error("Target not found in json data!");
	json empty;
	return empty;
}
