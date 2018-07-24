#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <utility>
#include "jsonutil.h"

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

char * convertToHexData(const char* str) {
	char * charbuffer = new char[strlen(str)];
	for (int i = 0; *str; ++str) {
		if (str[0] != ' ' && str[0] != '?') {
			charbuffer[i++] = (char)getByte(str);
			++str;
		}
		else if (str[0] == '?') {
			charbuffer[i++] = '\x00';
		}

	}
	return charbuffer;
}

void pik(const char* str) {
	size_t maxca = strlen(str);
	std::cout << "-----" << std::endl;
	for (; *str; ++str) {
		if (str[0] != ' ' && str[0] != '?') {
			std::cout << std::hex << getByte(str);
			++str;

		}
		else {
			std::cout << " ";
		}

	}
	std::cout << "\n-----" << std::endl;
}

int main()
{
	std::ifstream ifs("test.json");
	json j = json::parse(ifs);
	pik("81 F9 ? ? ? ? 75 1D F3 0F 10 05 ? ? ? ? F3 0F 11 44 24 ? 8B 44 24 1C 35 ? ? ? ? 89 44 24 18 EB 0B 8B 01 8B 40 30 FF D0 D9 5C 24 18 F3 0F 10 06");
	const char * mario = convertToHexData("81 F9 ? ? ? ? 75 1D F3 0F 10 05 ? ? ? ? F3 0F 11 44 24 ? 8B 44 24 1C 35 ? ? ? ? 89 44 24 18 EB 0B 8B 01 8B 40 30 FF D0 D9 5C 24 18 F3 0F 10 06");
	//const char * mario = "\x81\xf9\x00\x00\x00\x00\x75\x1d\xf3\xf\x10\x5\x00\x00\x00\x00\xf3\xf\x11\x44\x24\x00\x8b\x44\x24\x1c\x35\x00\x00\x00\x00\x89\x44\x24\x18\xeb\xb\x8b\x1\x8b\x40\x30\xff\xd0\xd9\x5c\x24\x18\xf3\xf\x10\x06";
	std::cout << "weee: " << mario << std::endl;
	std::cout << "\n" << "Its a me? ";
	for (; *mario; ++mario) {
		std::cout << std::hex << (int)(unsigned char)*mario;
	}
	std::cout << "\n";
	std::cout << "weee: " << "\x81\xf9\x00\x00\x00\x00\x75\x1d\xf3\xf\x10\x5\x00\x00\x00\x00\xf3\xf\x11\x44\x24\x00\x8b\x44\x24\x1c\x35\x00\x00\x00\x00\x89\x44\x24\x18\xeb\xb\x8b\x1\x8b\x40\x30\xff\xd0\xd9\x5c\x24\x18\xf3\xf\x10\x06" << std::endl;
	//\xBB\x00\x00\x00\x00\x83\xFF\x01\x0F\x8C\x00\x00\x00\x00\x3B\xF8

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