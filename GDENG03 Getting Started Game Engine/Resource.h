#pragma once
#include <unordered_map>
#include <string>
#include "Prerequisites.h"

class Resource
{
public:
	Resource(const wchar_t* resourceFullPath);
	virtual ~Resource();

protected:
	std::wstring resourceFullPath;

//private:
//	std::unordered_map<std::string, std::string> resources;
};

