#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <utility>

#include "jsonutil.h"
#include "patternscan.h"
#include "processtools.h"
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

			if (PatternElement["offsets"].size()) { // if there is any offsets, add them
				int totalOffset = 0;
				for (int i = 0; i != PatternElement["offsets"].size(); i++)
					totalOffset += PatternElement["offsets"][i];
				ReadProcessMemory(hProcess, LPCVOID((int)pointerAddress + totalOffset), &pBuffer, sizeof(uintptr_t), nullptr);
			}
			else {
				ReadProcessMemory(hProcess, LPCVOID((int)pointerAddress), &pBuffer, sizeof(uintptr_t), nullptr);
			}
#ifdef _DEBUG
			std::cout << "This offset is: " << name << "\n";
			std::cout << "Pattern as read from json: " << PatternElement["pattern"] << "\n";
			std::cout << std::hex << "Pointer Address: " << pointerAddress << "\n";
			std::cout << std::hex << "Value read from Pointer Address: " << (int)pBuffer << "\n";
			if (PatternElement["relative"]) {
				std::cout << std::hex << "Module Entry Address: " << (int)modEntry.modBaseAddr << "\n";
				std::cout << std::hex << "Offset Address from module entry.: " << ((int)pBuffer - (int)modEntry.modBaseAddr) << std::endl;
			}
#endif
			if (PatternElement["relative"])
				return (int)pBuffer - (int)modEntry.modBaseAddr;
			else
				return (int)pBuffer;
		}
	}
	catch (std::runtime_error& runErr) {
		std::cerr << runErr.what() << std::endl;
	}
	return NULL;
}