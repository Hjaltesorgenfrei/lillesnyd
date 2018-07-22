#include <nlohmann/json.hpp>
#include <iostream>


// for convenience
using json = nlohmann::json;


int main() {
	std::ifstream ifs("test.json");
	json j = json::parse(ifs);

	std::cout << j["pi"] << std::endl;

	std::cin.get();


}