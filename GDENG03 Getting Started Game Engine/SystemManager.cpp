#include "SystemManager.h"

SystemManager::SystemManager()
{
}

SystemManager::~SystemManager()
{
}

void SystemManager::entityDestroyed(EntityId entity)
{
    for (auto const& pair : systems)
    {
        auto const& system = pair.second;
        system->entities.erase(entity);
    }
}

void SystemManager::entitySignatureChanged(EntityId entity, std::bitset<MAX_COMPONENTS> entitySignature)
{
    for (auto const& pair : systems)
    {
        auto const& type = pair.first;
        auto const& system = pair.second;
        auto const& systemSignature = signatures[type];

        if ((entitySignature & systemSignature) == systemSignature)
        {
            system->entities.insert(entity);
        }
        else
        {
            system->entities.erase(entity);
        }
    }
}