#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <utility>
#include <string>

#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))


DWORD GetProcID(const wchar_t * exeName);

MODULEENTRY32 GetModule(DWORD dwProcID, const wchar_t * moduleName);

std::vector<std::string> StringToVector(std::string const& str, char const delimiter);

std::vector<std::pair<char, bool>> convertToHexVector(const char* str);