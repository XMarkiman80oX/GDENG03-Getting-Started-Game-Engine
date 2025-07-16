#include "GraphicsEngine.h"
#include "RenderSystem.h"
#include <exception>

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
}

GraphicsEngine::~GraphicsEngine()
{
	GraphicsEngine::m_engine = nullptr;

	delete this->m_render_system;
	delete this->m_texture_manager;
}


GraphicsEngine* GraphicsEngine::get()
{
	return m_engine;
}

void GraphicsEngine::create()
{
	if(GraphicsEngine::m_engine)
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
