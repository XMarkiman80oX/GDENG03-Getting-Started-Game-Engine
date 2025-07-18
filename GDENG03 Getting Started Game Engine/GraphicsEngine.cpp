#include "GraphicsEngine.h"
#include "GameObjectManager.h"
#include <exception>

#include "ComponentManager.h"
#include "SystemManager.h"

GraphicsEngine* GraphicsEngine::m_engine = nullptr;

GraphicsEngine::GraphicsEngine()
{
	try {

		this->m_render_system = new RenderSystem();
	}
	catch (...) {
		throw std::exception("Failed to initialize the graphics engine. Make sure you have a valid graphics driver installed.");
	}

	try {

		this->m_texture_manager = new TextureManager();
	}
	catch (...) {
		throw std::exception("Texture Manager not created successfully");
	}

	try {

		this->m_mesh_manager = new MeshManager();
	}
	catch (...) {
		throw std::exception("Mesh Manager not created successfully");
	}


	/*----------------VERTEX MESH LAYOUT SHADER PART----------------*/
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	this->m_render_system->compileVertexShader(L"VertexMeshLayoutShader.hlsl", "main", &shader_byte_code, &size_shader);

	::memcpy(m_mesh_layout_byte_code, shader_byte_code, size_shader);

	this->m_mesh_layout_size = size_shader;

	this->m_render_system->releaseCompiledShader();
	/*--------------------------------------------------------------*/

	// Initialize ECS Managers
	componentManager = std::make_unique<ComponentManager>();
	gameObjectManager = std::make_unique<GameObjectManager>();
	systemManager = std::make_unique<SystemManager>();
}

GraphicsEngine::~GraphicsEngine()
{
	GraphicsEngine::m_engine = nullptr;

	delete this->m_render_system;
	delete this->m_texture_manager;
	delete this->m_mesh_manager;
}


GraphicsEngine* GraphicsEngine::getInstance()
{
	return m_engine;
}

void GraphicsEngine::create()
{
	if (GraphicsEngine::m_engine)
		throw std::exception("GraphicsEngine has already been created. Don't call GraphicsEngine::create() anymore.");

	GraphicsEngine::m_engine = new GraphicsEngine();
}

void GraphicsEngine::release()
{
	if (!GraphicsEngine::m_engine)
		return;

	delete GraphicsEngine::m_engine;
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return this->m_render_system;
}

TextureManager* GraphicsEngine::getTextureManager()
{
	return this->m_texture_manager;
}

MeshManager* GraphicsEngine::getMeshManager()
{
	return this->m_mesh_manager;
}

void GraphicsEngine::getVertexMeshLayoutShaderByteCodeAndSize(void** byte_code, size_t* size)
{
	*byte_code = this->m_mesh_layout_byte_code;
	*size = this->m_mesh_layout_size;
}

// --- ECS Method Implementations ---

void GraphicsEngine::destroyEntity(EntityId entity)
{
	gameObjectManager->destroyEntity(entity);
	componentManager->entityDestroyed(entity);
	systemManager->entityDestroyed(entity);
}
template<typename T>
void GraphicsEngine::registerComponent() 
{ 
	componentManager->registerComponent<T>(); 
}

template<typename T>
void GraphicsEngine::addComponent(EntityId entity, T component) {
	componentManager->addComponent<T>(entity, component);
	auto signature = gameObjectManager->getSignature(entity); // No more undefined type error
	signature.set(componentManager->getComponentType<T>(), true);
	gameObjectManager->setSignature(entity, signature);
	systemManager->entitySignatureChanged(entity, signature);
}

template<typename T>
void GraphicsEngine::removeComponent(EntityId entity) {
	componentManager->removeComponent<T>(entity);
	auto signature = gameObjectManager->getSignature(entity);
	signature.set(componentManager->getComponentType<T>(), false);
	gameObjectManager->setSignature(entity, signature);
	systemManager->entitySignatureChanged(entity, signature);
}

template<typename T>
T& GraphicsEngine::getComponent(EntityId entity)
{ 
	return componentManager->getComponent<T>(entity); 
}

template<typename T>
ComponentId GraphicsEngine::getComponentType()
{
	return componentManager->getComponentType<T>(); 
}

template<typename T>
std::shared_ptr<T> GraphicsEngine::registerSystem()
{
	return systemManager->registerSystem<T>(); 
}

template<typename T>
void GraphicsEngine::setSystemSignature(std::bitset<MAX_COMPONENTS> signature)
{
	systemManager->setSignature<T>(signature); 
}