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


int GetOffsetFromJson(json& j, std::string name, const wchar_t * exeName, const wchar_t* moduleName) {
	try {
		json PatternElement = jsonArrayValueFinder(j, "signatures", "name", name);

		std::string patternString = PatternElement["pattern"];
		//TODO: Change this so that it does not create this hex vector first.
		std::vector<std::pair<char, bool>> PatternVector = convertToHexVector(patternString.c_str());
		//Get Process ID by enumerating the processes using tlhelp32snapshot
		DWORD processID = GetProcID(exeName);
		//Get handle by OpenProcess
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
		{
			MODULEENTRY32 modEntry = GetModule(processID, moduleName);
			//PatternScan
			char * patternChar = VectorToPattern(PatternVector);
			char * mask = VectorToMask(PatternVector);
			void * pointerAddress = PatternScanExModule(hProcess, exeName, moduleName, patternChar, mask);
			LPVOID pBuffer;
			const uintptr_t& point = (int)pointerAddress + PatternElement["offsets"][0]; //Gets an off by one error i think. 
			ReadProcessMemory(hProcess, LPCVOID(point), &pBuffer, sizeof(uintptr_t), nullptr);
			
			#ifdef _DEBUG
			std::cout << "This offset is: " << name << "\n";
			std::cout << "Pattern as read from json: " << PatternElement["pattern"] << "\n";
			std::cout << std::hex << "Pointer Address: " << pointerAddress << "\n";
			std::cout << std::hex << "Module Entry Address: " << (int)modEntry.modBaseAddr << "\n";
			std::cout << std::hex << "Offset Address: " << (int)pBuffer - (int)modEntry.modBaseAddr << std::endl;
			#endif
			
			return (int)pBuffer - (int)modEntry.modBaseAddr;
		}
	}
	catch (std::runtime_error& runErr) {
		std::cerr << runErr.what() << std::endl;
	}
	return NULL;
}



int GetOffsetFromJson(json& j, std::string name) {
	try {

		json PatternElement = jsonArrayValueFinder(j, "signatures", "name", name);

		std::string str = j["executable"];
		std::wstring widestr = std::wstring(str.begin(), str.end());
		const wchar_t * exeName = widestr.c_str();

		std::string str2 = PatternElement["module"];
		std::wstring widestr2 = std::wstring(str2.begin(), str2.end());
		const wchar_t * moduleName = widestr2.c_str();
		
		std::string patternString = PatternElement["pattern"];
		//TODO: Change this so that it does not create this hex vector first.
		std::vector<std::pair<char, bool>> PatternVector = convertToHexVector(patternString.c_str());
		//Get Process ID by enumerating the processes using tlhelp32snapshot
		DWORD processID = GetProcID(exeName);
		//Get handle by OpenProcess
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
		{
			MODULEENTRY32 modEntry = GetModule(processID, moduleName);
			//PatternScan
			char * patternChar = VectorToPattern(PatternVector);
			char * mask = VectorToMask(PatternVector);
			void * pointerAddress = PatternScanExModule(hProcess, exeName, moduleName, patternChar, mask);
			LPVOID pBuffer;
			const uintptr_t& point = (int)pointerAddress + PatternElement["offsets"][0]; //Gets an off by one error i think. 
			ReadProcessMemory(hProcess, LPCVOID(point), &pBuffer, sizeof(uintptr_t), nullptr);

			#ifdef _DEBUG
			std::cout << "This offset is: " << name << "\n";
			std::cout << "Pattern as read from json: " << PatternElement["pattern"] << "\n";
			std::cout << std::hex << "Pointer Address: " << pointerAddress << "\n";
			std::cout << std::hex << "Module Entry Address: " << (int)modEntry.modBaseAddr << "\n";
			std::cout << std::hex << "Offset Address: " << (int)pBuffer - (int)modEntry.modBaseAddr << std::endl;
			#endif
			
			return (int)pBuffer - (int)modEntry.modBaseAddr;
		}
	}
	catch (std::runtime_error& runErr) {
		std::cerr << runErr.what() << std::endl;
	}
	return NULL;
}

int main()
{


	std::ifstream ifs("config.json");
	json j = json::parse(ifs);

	GetOffsetFromJson(j, "dwLocalPlayer", L"csgo.exe", L"client.dll");
	GetOffsetFromJson(j, "dwEntityList");

	for (auto element : j["signatures"]) {
		GetOffsetFromJson(j, element["name"]);
		system("pause");
	}

	std::cin.get();
	return 0;

}