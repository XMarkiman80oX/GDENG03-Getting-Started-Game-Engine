#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
{
    for (EntityId entity = 0; entity < MAX_ENTITIES; ++entity)
    {
        availableEntities.push(entity);
    }
}

GameObjectManager::~GameObjectManager()
{
}

GameObjectManager* GameObjectManager::getInstance()
{
    return gameObjectManager;
}

void GameObjectManager::create()
{
    if (GameObjectManager::gameObjectManager)
        throw std::exception("GameObjectManager has already been created. Don't call GameObjectManager::create() anymore.");

    GameObjectManager::gameObjectManager = new GameObjectManager();
}

void GameObjectManager::release()
{
    if (!GameObjectManager::gameObjectManager)
        return;

    delete GameObjectManager::gameObjectManager;
}

void GameObjectManager::createGameObject(BaseGameObject* spawningObject)
{
    void* shaderByteCode = nullptr;
    size_t sizeShader = 0;

    EntityId entity = this->generateEntityID();

    TransformComponent transform;
    transform.position = spawningObject->getLocalPosition();
    transform.rotation = spawningObject->getLocalRotation();
    transform.scale = spawningObject->getLocalScale();

    GraphicsEngine::getInstance()->addComponent(entity, transform);

    RenderComponent render;
    const wchar_t* meshPath = spawningObject->getMeshPath();
    const wchar_t* texturePath = spawningObject->getTexturePath();

    try
    {
        render.mesh = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(meshPath);
    }
    catch(...)
    {
        throw std::exception("Object Mesh not set successfully");
    }
    try 
    {
        render.texture = GraphicsEngine::getInstance()->getTextureManager()->createTextureFromFile(texturePath);
    }
    catch (...) 
    {
        throw std::exception("Object Texture not set successfully");
    }


    GraphicsEngine::getInstance()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);
    render.vertexShader = GraphicsEngine::getInstance()->getRenderSystem()->createVertexShader(shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

    GraphicsEngine::getInstance()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);
    render.pixelShader = GraphicsEngine::getInstance()->getRenderSystem()->createPixelShader(shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

    constantBufferData cbd = {};
    cbd.m_time = 0;
    render.constantBuffer = GraphicsEngine::getInstance()->getRenderSystem()->createConstantBuffer(&cbd, sizeof(constantBufferData));

    GraphicsEngine::getInstance()->addComponent(entity, render);
}

EntityId GameObjectManager::generateEntityID()
{
    EntityId id = availableEntities.front();
    availableEntities.pop();
    activeEntityCount++;
    return id;
}

void GameObjectManager::setSignature(EntityId entity, std::bitset<MAX_COMPONENTS> signature)
{
    signatures[entity] = signature;
}

std::bitset<MAX_COMPONENTS> GameObjectManager::getSignature(EntityId entity)
{
    return signatures[entity];
}

void GameObjectManager::destroyEntity(EntityId entity)
{
    signatures[entity].reset();
    activeEntityCount--;
    availableEntities.push(entity);
}