#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <utility>

#include "jsonutil.h"
#include "patternscan.h"
#include "processtools.h"




int main()
{


	std::ifstream ifs("config.json");
	json j = json::parse(ifs);

	GetOffsetFromJson(j, "dwLocalPlayer");

	for (auto element : j["signatures"]) {
		if (element["relative"])
			GetOffsetFromJson(j, element["name"]);
	}
	std::cout << "______________" << std::endl;
	for (auto element : j["signatures"]) {
		if (!element["relative"])
			GetOffsetFromJson(j, element["name"]);
	}

	std::cin.get();
	return 0;

}