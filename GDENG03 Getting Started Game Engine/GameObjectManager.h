#pragma once
#include "SystemAttributes.h"
#include "GraphicsEngine.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ConstantBufferData.h"
#include "BaseGameObject.h"
#include "Vector3D.h"
#include <queue>
#include <array>
#include <bitset>

class GameObjectManager
{
public:
    GameObjectManager();
    ~GameObjectManager();

    void createGameObject(BaseGameObject* spawningObject);

    EntityId generateEntityID();

public:
    static GameObjectManager* getInstance();
    static void create();
    static void release();

public:
    void setSignature(EntityId entity, std::bitset<MAX_COMPONENTS> signature); 

public:
    std::bitset<MAX_COMPONENTS> getSignature(EntityId entity);

public:
    void destroyEntity(EntityId entity);

private:
    static GameObjectManager* gameObjectManager;

private:
    std::queue<EntityId> availableEntities{};
    // An array of bitsets to store the component signature for each entity.
    std::array<std::bitset<MAX_COMPONENTS>, MAX_ENTITIES> signatures{};
    uint32_t activeEntityCount{};
};