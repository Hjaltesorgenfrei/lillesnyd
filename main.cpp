#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>

// for convenience
using json = nlohmann::json;


int main() {
	std::ifstream ifs("test.json");
	json j = json::parse(ifs);

	std::cout << j["pi"] << std::endl;


	std::cin.get();
	// add a string that is stored as std::string
	j["name"] = "Niels";

	// add another null object by passing nullptr
	j["nothing"] = nullptr;

	// add an object inside the object
	j["answer"]["everything"] = 42;

	// add an array that is stored as std::vector (using an initializer list)
	j["list"] = { 1, 0, 2 };

	// add another object (using an initializer list of pairs)
	j["object"] = { { "currency", "USD" },{ "value", 42.99 } };

	// write prettified JSON to another file
	std::ofstream o("pretty.json");
	o << std::setw(4) << j << std::endl;
}