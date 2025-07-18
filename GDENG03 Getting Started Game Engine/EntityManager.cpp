#include "EntityManager.h"

EntityManager::EntityManager()
{
    for (EntityId entity = 0; entity < MAX_ENTITIES; ++entity)
    {
        availableEntities.push(entity);
    }
}

EntityManager::~EntityManager()
{
}

EntityId EntityManager::createEntity()
{
    EntityId id = availableEntities.front();
    availableEntities.pop();
    activeEntityCount++;
    return id;
}

void EntityManager::setSignature(EntityId entity, std::bitset<MAX_COMPONENTS> signature)
{
    signatures[entity] = signature;
}

std::bitset<MAX_COMPONENTS> EntityManager::getSignature(EntityId entity)
{
    return signatures[entity];
}

void EntityManager::destroyEntity(EntityId entity)
{
    signatures[entity].reset();
    activeEntityCount--;
    availableEntities.push(entity);
}