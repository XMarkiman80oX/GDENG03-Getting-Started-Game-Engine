#include "WorldCamera.h"
#include "InputSystem.h"
#include "AppWindow.h"
#include "Vector3D.h"
#include "ConstantBufferData.h"
#include <iostream>
#include "GraphicsEngine.h"

WorldCamera::WorldCamera()
{
}

WorldCamera::~WorldCamera()
{
}

WorldCamera* WorldCamera::getInstance()
{
    static WorldCamera camera;
    return &camera;
}

void WorldCamera::initialize(RECT windowRect)
{
    this->windowRect = windowRect;

    this->worldCamera.setIdentity();
    this->worldCamera.setTranslation(Vector3D(0.0f, 0.0f, -2.0f));

    // Initialize the projection matrix
    int width = (windowRect.right - windowRect.left);
    int height = (windowRect.bottom - windowRect.top);
    this->m_proj.setPerspectiveFOVLH(1.57f, (float)width / (float)height, 0.1f, 100.0f);

    InputSystem::getInstance()->addListener(this);
}

Matrix4x4 WorldCamera::getWorldCamera()
{
    return this->worldCamera;
}

void WorldCamera::updateCamera()
{
    Matrix4x4 worldCam;
    worldCam.setIdentity();

    Matrix4x4 temp;

    // PITCH: Rotation around the X-axis
    temp.setIdentity();
    temp.setRotationX(this->rotationX);
    worldCam *= temp;

    // YAW: Rotation around the Y-axis
    temp.setIdentity();
    temp.setRotationY(this->rotationY);
    worldCam *= temp;

    Vector3D newPos = this->worldCamera.getTranslation() + worldCam.getZDirection() * (this->forward * 0.3f);
    newPos = newPos + worldCam.getXDirection() * (this->rightward * 0.3f);

    worldCam.setTranslation(newPos);
    this->worldCamera = worldCam;

    // Update the projection matrix (in case the window is resized)
    int width = (windowRect.right - windowRect.left);
    int height = (windowRect.bottom - windowRect.top);
    this->m_proj.setPerspectiveFOVLH(1.57f, (float)width / (float)height, 0.1f, 100.0f);
}

Matrix4x4 WorldCamera::getProjectionMatrix()
{
    return this->m_proj;
}

Vector3D WorldCamera::getTranslation()
{
    return this->worldCamera.getTranslation();
}
Vector3D WorldCamera::getZDirection()
{
    return this->worldCamera.getZDirection();
}
Vector3D WorldCamera::getXDirection()
{
    return this->worldCamera.getXDirection();
}
Matrix4x4 WorldCamera::getViewMatrix()
{
    Matrix4x4 view = this->worldCamera;
    view.setInverse();
    return view;
}
void WorldCamera::setInverse()
{
    this->worldCamera.setInverse();
}

void WorldCamera::onKeyDown(int key)
{
    switch (key) {
    case 'W':
        this->forward = 1.0f;
        break;
    case 'S':
        this->forward = -1.0f;
        break;
    case 'A':
        this->rightward = -1.0f;
        break;
    case 'D':
        this->rightward = 1.0f;
        break;
    }
}

void WorldCamera::onKeyUp(int key)
{
    this->forward = 0.0f;
    this->rightward = 0.0f;
}

void WorldCamera::onMouseMove(const Point& mousePosition)
{
    float deltaTime = static_cast<float>(EngineTime::getDeltaTime());

    int width = (this->windowRect.right - this->windowRect.left);
    int height = (this->windowRect.bottom - this->windowRect.top);

    float incrementerX = this->sensitivity * (mousePosition.x - (width / 2.0f)) * deltaTime;
    float incrementerY = this->sensitivity * (mousePosition.y - (height / 2.0f)) * deltaTime;

    if (this->invertedIsOn)
    {
        this->rotationX -= incrementerY;
        this->rotationY -= incrementerX;
    }
    else
    {
        this->rotationX += incrementerY;
        this->rotationY += incrementerX;
    }

    InputSystem::getInstance()->setCursorPosition(Point(width / 2.0f, height / 2.0f));
}

void WorldCamera::onLeftMouseDown(const Point& mousePosition)
{
}

void WorldCamera::onLeftMouseUp(const Point& mousePosition)
{
}

void WorldCamera::onRightMouseDown(const Point& mousePosition)
{
}

void WorldCamera::onRightMouseUp(const Point& mousePosition)
{
}