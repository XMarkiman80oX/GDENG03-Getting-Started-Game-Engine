#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
#include "ConstantBufferData.h"

AppWindow::AppWindow()
{
}

AppWindow::~AppWindow()
{
}

AppWindow* AppWindow::getInstance()
{
	static AppWindow appWindow;
	return &appWindow;
}

void AppWindow::onCreate()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	//We need to add AppWindow as a listener to the Input System
	InputSystem::getInstance()->addListener(this);
	InputSystem::getInstance()->showCursor(this->cursorIsVisible);

	// Initialize Physics World
	m_physicsWorld = m_physicsCommon.createPhysicsWorld();
	m_physicsWorld->setGravity(reactphysics3d::Vector3(0.0, -9.81, 0.0));

	// Create a static rigid body for the plane and associate it
	reactphysics3d::RigidBody* planeBody = m_physicsWorld->createRigidBody(reactphysics3d::Transform({ 0, -1, 0 }, reactphysics3d::Quaternion::identity()));
	planeBody->setType(reactphysics3d::BodyType::STATIC);
	reactphysics3d::BoxShape* planeShape = m_physicsCommon.createBoxShape(reactphysics3d::Vector3(100.0, 1.0, 100.0));
	planeBody->addCollider(planeShape, reactphysics3d::Transform::identity());
	objectsInWorld[1]->setRigidBody(planeBody);

	/*try {
		this->m_mesh = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\teapot.obj");
		this->m_mesh2 = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\bunny.obj");
		this->m_mesh3 = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\armadillo.obj");
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, L"Failed to load mesh.", L"Error", MB_OK);
	}*/

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain = GraphicsEngine::getInstance()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);
	this->m_depth_buffer = GraphicsEngine::getInstance()->getRenderSystem()->createDepthBuffer(rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);


	WorldCamera::getInstance()->initialize(rc);
	WorldCamera::getInstance()->setTranslation(Vector3D(0, 0, -2));

	Cube* marcosCube = new Cube("Marco's Cube", shader_byte_code, size_shader, GraphicsEngine::getInstance()->getRenderSystem());
	Plane* marcosPlane = new Plane("Marco's Plane", shader_byte_code, size_shader, GraphicsEngine::getInstance()->getRenderSystem());

	marcosPlane->setPosition(Vector3D(0, 0, 2));
	marcosPlane->setScale(100);
	this->objectsInWorld.push_back(marcosCube);
	this->objectsInWorld.push_back(marcosPlane);
}

void AppWindow::onUpdate()
{
	//Inputs get processed here
	InputSystem::getInstance()->update();

	this->deltaTime = static_cast<float>(EngineTime::getDeltaTime());

	GraphicsEngine::getInstance()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor
	(
		this->m_swap_chain,
		this->m_depth_buffer,
		0, 0.3f, 0.4f, 1
	);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::getInstance()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);


	WorldCamera::getInstance()->updateCamera();

	this->updateGameObjects(rc);

	// Update physics world
	m_physicsWorld->update(this->deltaTime);

	// Update game object transforms from physics simulation
	for (auto& object : objectsInWorld)
	{
		if (object->getRigidBody() && object->getRigidBody()->getType() == reactphysics3d::BodyType::DYNAMIC)
		{
			const reactphysics3d::Transform& transform = object->getRigidBody()->getTransform();
			object->setPosition(transform.getPosition().x, transform.getPosition().y, transform.getPosition().z);

			reactphysics3d::Quaternion quat = transform.getOrientation();
			reactphysics3d::Matrix3x3 matrix = quat.getMatrix();
			float pitch = asin(-matrix[2][1]);
			float yaw = atan2(matrix[2][0], matrix[2][2]);
			float roll = atan2(matrix[0][1], matrix[1][1]);
			object->setRotation(pitch, yaw, roll);
		}
	}
	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_physicsCommon.destroyPhysicsWorld(m_physicsWorld);
	GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();
}

void AppWindow::onFocus()
{
	InputSystem::getInstance()->addListener(this);
	InputSystem::getInstance()->addListener(WorldCamera::getInstance());
}

void AppWindow::onKillFocus()
{
	InputSystem::getInstance()->removeListener(this);
	InputSystem::getInstance()->removeListener(WorldCamera::getInstance());
}


void AppWindow::onLeftMouseDown(const Point& mousePosition)
{
}

void AppWindow::onLeftMouseUp(const Point& mousePosition)
{
}

void AppWindow::onRightMouseDown(const Point& mousePosition)
{
}

void AppWindow::onRightMouseUp(const Point& mousePosition)
{
}

void AppWindow::updateGameObjects(RECT clientWindowRect)
{
	for(BaseGameObject* object : this->objectsInWorld)
	{
		object->update(clientWindowRect);
		object->draw(clientWindowRect.right - clientWindowRect.left, clientWindowRect.bottom - clientWindowRect.top);
	}
}

void AppWindow::destroyGameObjects()
{
	this->objectsInWorld.clear();
}

void AppWindow::selectNextObject()
{
	this->objectSelectedIndex++;

	//Make sure it doesn't go over the amount of objects in this->objectsInWorld
	if (this->objectSelectedIndex > this->objectsInWorld.size() - 1)
		this->objectSelectedIndex = 0;

	//Set All Selected Objects to false first before toggling it on
	for (BaseGameObject* object : this->objectsInWorld) {
		this->objectsInWorld[this->objectSelectedIndex]->setSelected(false);
	}

	this->objectsInWorld[this->objectSelectedIndex]->setSelected(true);
}

void AppWindow::onMouseMove(const Point& mousePosition)
{
}

void AppWindow::onKeyDown(int key)
{
}

void AppWindow::onKeyUp(int key)
{
	switch (key) {
	case VK_SPACE:
		this->selectNextObject();
		this->spawnCubes();
		break;
	}
}
void AppWindow::spawnCubes()
{
	for (int i = 0; i < 20; ++i)
	{
		// Create a cube game object
		Cube* cube = new Cube("FallingCube", m_cube_vs_byte_code, m_cube_vs_size, GraphicsEngine::getInstance()->getRenderSystem());
		float x = -10.0f + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (20.0f)));
		float z = -10.0f + static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / (20.0f)));
		cube->setPosition(x, 20.0f, z); // Spawn from the sky
		this->objectsInWorld.push_back(cube);

		// Create a dynamic rigid body for the cube
		reactphysics3d::Transform transform({ x, 20.0f, z }, reactphysics3d::Quaternion::identity());
		reactphysics3d::RigidBody* cubeBody = m_physicsWorld->createRigidBody(transform);
		cubeBody->setType(reactphysics3d::BodyType::DYNAMIC);
		reactphysics3d::BoxShape* cubeShape = m_physicsCommon.createBoxShape(reactphysics3d::Vector3(0.5, 0.5, 0.5));
		cubeBody->addCollider(cubeShape, reactphysics3d::Transform::identity());
		cube->setRigidBody(cubeBody);
	}
}