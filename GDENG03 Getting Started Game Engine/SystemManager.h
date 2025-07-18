#pragma once
#include "SystemAttributes.h"
#include "System.h"
#include <memory>
#include <unordered_map>
#include <bitset>

class SystemManager
{
private:
    std::unordered_map<const char*, std::bitset<MAX_COMPONENTS>> signatures{};
    std::unordered_map<const char*, std::shared_ptr<System>> systems{};

public:
	SystemManager();
	virtual ~SystemManager();

public:
    template<typename T>
    std::shared_ptr<T> registerSystem()
    {
        const char* typeName = typeid(T).name();
        auto system = std::make_shared<T>();
        systems.insert({ typeName, system });
        return system;
    }

    template<typename T>
    void setSignature(std::bitset<MAX_COMPONENTS> signature)
    {
        const char* typeName = typeid(T).name();
        signatures.insert({ typeName, signature });
    }

    void entityDestroyed(EntityId entity);

    void entitySignatureChanged(EntityId entity, std::bitset<MAX_COMPONENTS> entitySignature);
};