#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "jsonutil.h"


int main() 
{
	std::ifstream ifs("test.json");
	json j = json::parse(ifs);
	

	try {
	json localplayer = jsonArrayValueFinder(j, "signatures", "name", "dwLocalPlayer");
	std::cout << localplayer["pattern"] << std::endl;
	}
	catch (std::runtime_error& runErr) {
		std::cerr << runErr.what() << std::endl;
	}

	
	
	std::cin.get();
	return 0;

}