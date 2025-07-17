#include "ResourceManager.h"
#include <filesystem>
#include <iostream>

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

ResourcePtr ResourceManager::createResourceFromFile(const wchar_t* file_path)
{
    std::wstring full_path = std::filesystem::absolute(file_path);
    //std::wstring full_path = std::experimental::filesystem::absolute(file_path);

	auto it = this->m_map_resources.find(full_path);

    if(it != this->m_map_resources.end())
        return it->second;
	
	Resource* raw_resource = this->createResourceFromFileConcrete(full_path.c_str());

    if(raw_resource)
    {
        std::cout << "RESOURCE IS NOT NULL" << std::endl;
        ResourcePtr resource_ptr(raw_resource);
        this->m_map_resources[full_path] = resource_ptr;
        return resource_ptr;
	}
    else {
        std::cout << "RESOURCE IS NULL" << std::endl;
        return nullptr;
    }
}
