#include "GraphicsEngine.h"


GraphicsEngine::GraphicsEngine()
{
}

bool GraphicsEngine::init()
{
	this->m_render_system = new RenderSystem();
	this->m_render_system->init();
	return true;
}

bool GraphicsEngine::release()
{
	this->m_render_system->release();
	delete this->m_render_system;
	return true;
}

GraphicsEngine::~GraphicsEngine()
{
}


GraphicsEngine* GraphicsEngine::get()
{
	static GraphicsEngine engine;
	return &engine;
}

RenderSystem* GraphicsEngine::getRenderSystem()
{
	return this->m_render_system;
}
