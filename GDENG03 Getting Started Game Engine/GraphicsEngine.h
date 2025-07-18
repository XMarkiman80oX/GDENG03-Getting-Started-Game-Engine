#pragma once
#include <d3d11.h>
#include "Prerequisites.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "MeshManager.h"
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
	void destroyEntity(EntityId entity);

	template<typename T>
	void registerComponent();

	template<typename T>
	void addComponent(EntityId entity, T component);

	template<typename T>
	void removeComponent(EntityId entity);

	template<typename T>
	T& getComponent(EntityId entity);

	template<typename T>
	ComponentId getComponentType();

	template<typename T>
	std::shared_ptr<T> registerSystem();

	template<typename T>
	void setSystemSignature(std::bitset<MAX_COMPONENTS> signature);

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
	ComponentManagerPtr componentManager;
	GameObjectManagerPtr gameObjectManager; // Fully defined type is now available
	SystemManagerPtr systemManager;
};