#pragma once
#include "SystemAttributes.h"
#include <queue>
#include <array>
#include <bitset>

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

    EntityId createEntity();

public:
    void setSignature(EntityId entity, std::bitset<MAX_COMPONENTS> signature); 

public:
    std::bitset<MAX_COMPONENTS> getSignature(EntityId entity);

public:
    void destroyEntity(EntityId entity);

private:
    std::queue<EntityId> availableEntities{};
    // An array of bitsets to store the component signature for each entity.
    std::array<std::bitset<MAX_COMPONENTS>, MAX_ENTITIES> signatures{};
    uint32_t activeEntityCount{};
};