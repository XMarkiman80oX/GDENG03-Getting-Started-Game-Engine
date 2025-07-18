#pragma once
#include <d3d11.h>
#include "Prerequisites.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
#include <bitset>

class GraphicsEngine
{
private:
	GraphicsEngine();
	~GraphicsEngine();

public:
	RenderSystem* getRenderSystem();
	TextureManager* getTextureManager();
	MeshManager* getMeshManager();
	void getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size);

	// --- ECS Methods ---
	void initECS();
	EntityId createEntity();
	void destroyEntity(EntityId entity);

	template<typename T>
	void registerComponent() { componentManager->registerComponent<T>(); }

	template<typename T>
	void addComponent(EntityId entity, T component) {
		componentManager->addComponent<T>(entity, component);
		auto signature = entityManager->getSignature(entity);
		signature.set(componentManager->getComponentType<T>(), true);
		entityManager->setSignature(entity, signature);
		systemManager->entitySignatureChanged(entity, signature);
	}

	template<typename T>
	void removeComponent(EntityId entity) {
		componentManager->removeComponent<T>(entity);
		auto signature = entityManager->getSignature(entity);
		signature.set(componentManager->getComponentType<T>(), false);
		entityManager->setSignature(entity, signature);
		systemManager->entitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& getComponent(EntityId entity) { return componentManager->getComponent<T>(entity); }

	template<typename T>
	ComponentId getComponentType() { return componentManager->getComponentType<T>(); }

	template<typename T>
	std::shared_ptr<T> registerSystem() { return systemManager->registerSystem<T>(); }

	template<typename T>
	void setSystemSignature(std::bitset<MAX_COMPONENTS> signature) { systemManager->setSignature<T>(signature); }

public:
	static GraphicsEngine* getInstance();
	static void create();
	static void release();

private:
	RenderSystem* m_render_system = nullptr;
	TextureManager* m_texture_manager = nullptr;
	MeshManager* m_mesh_manager = nullptr;
	static GraphicsEngine* m_engine;
	unsigned char m_mesh_layout_byte_code[1024];
	size_t m_mesh_layout_size = 0;

	// --- ECS Members ---
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<SystemManager> systemManager;
};