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
		//Get Process ID by enumerating the processes using tlhelp32snapshot
		DWORD processID = GetProcID(L"csgo.exe");
		//Get handle by OpenProcess
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
		{
			MODULEENTRY32 modEntry = GetModule(processID, L"client.dll");
			std::cout << "THIS IS THE LOCALPLAYER" << std::endl;
			//PatternScan
			char * patternChar = VectorToPattern(realnibba);
			char * mask = VectorToMask(realnibba);
			void * healthDecAddress = PatternScanExModule(hProcess, L"csgo.exe", L"client.dll", patternChar, mask);
			std::cout << std::hex << healthDecAddress << std::endl;
			LPVOID pBuffer;
			const uintptr_t& point = (int)healthDecAddress + 1; //Gets an off by one error i think. 
			ReadProcessMemory(hProcess, LPCVOID(point), &pBuffer, sizeof(uintptr_t), nullptr);
			std::cout << std::hex << (int)modEntry.modBaseAddr << std::endl;
			std::cout << std::hex << (int)pBuffer - (int)modEntry.modBaseAddr << std::endl;
		}
	}
	catch (std::runtime_error& runErr) {
		std::cerr << runErr.what() << std::endl;
	}


	std::cin.get();
	return 0;

}