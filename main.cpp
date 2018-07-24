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

#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))


std::vector<std::string> StringToVector(std::string const& str, char const delimiter) {

	std::vector<std::string> vec;
	std::string element;


	//we are going to loop through each character of the string slowly building an element string.
	//whenever we hit a delimiter, we will push the element into the vector, and clear it to get ready for the next element
	for_each(begin(str), end(str), [&](char const ch) {
		if (ch != delimiter) {
			element += ch;
		}
		else {
			if (element.length() > 0) {
				vec.push_back(element);
				element.clear();
			}
		}
	});


	//push in the last element if the string does not end with the delimiter
	if (element.length() > 0) {
		vec.push_back(element);
	}


	return vec;
}

std::vector<std::pair<char, bool>> convertToHexVector(const char* str) {
	std::vector<std::pair<char, bool>> buffer;
	for (; *str; ++str) {
		if (str[0] != ' ' && str[0] != '?') {
			buffer.push_back(std::make_pair((char)getByte(str), true));
			++str;
		}
		else if (str[0] == '?') {
			buffer.push_back(std::make_pair('\x00', false));
		}

	}
	return buffer;
}


int main()
{
	std::ifstream ifs("test.json");
	json j = json::parse(ifs);

	try {
		json localplayer = jsonArrayValueFinder(j, "signatures", "name", "dwLocalPlayer");
		std::cout << localplayer["pattern"] << std::endl;
		std::vector<std::string> vec{ StringToVector(localplayer["pattern"], ' ') };
		std::string stdq = localplayer["pattern"];
		//print the vector
		for (std::string const& item : vec) {
			std::cout << "[" << item << "]";
		}
		std::cout << std::endl;
		std::vector<std::pair<char, bool>> realnibba = convertToHexVector(stdq.c_str());
		for (std::pair<char, bool> ch : realnibba)
		{
			if (std::get<bool>(ch))
				std::cout << std::hex << (int)(unsigned char)std::get<char>(ch) << " ";
			else std::cout << "? ";
		}
	}
	catch (std::runtime_error& runErr) {
		std::cerr << runErr.what() << std::endl;
	}


	std::cin.get();
	return 0;

}