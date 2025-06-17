#include "WorldCamera.h"

WorldCamera::WorldCamera()
{
    this->bufferCamera.setIdentity();
    this->worldCamera.setIdentity();
}

WorldCamera::~WorldCamera()
{
}

WorldCamera* WorldCamera::getInstance()
{
    static WorldCamera camera;
    return &camera;
}

Matrix4x4 WorldCamera::getWorldCamera()
{
    return this->worldCamera;
}

void WorldCamera::applyCameraChanges()
{
    this->worldCamera = this->bufferCamera;
}

void WorldCamera::setRotationX(float rotationValueX)
{
    this->bufferCamera.setIdentity();
    Matrix4x4 temp;

    temp.setIdentity();
    temp.setRotationX(rotationValueX);
    this->bufferCamera *= temp;
}

void WorldCamera::setRotationY(float rotationValueY)
{
    Matrix4x4 temp;

    temp.setIdentity();
    temp.setRotationY(rotationValueY);
    this->bufferCamera *= temp;
}
void WorldCamera::moveThroughX(float moveUnits)
{
    Vector3D newPos = this->worldCamera.getTranslation() + this->bufferCamera.getXDirection() * (moveUnits * 0.3f);

    this->worldCamera.setTranslation(newPos);
}
void WorldCamera::moveThroughZ(float moveUnits)
{
    Vector3D newPos = this->worldCamera.getTranslation() + this->bufferCamera.getZDirection() * (moveUnits * 0.3f);

    this->worldCamera.setTranslation(newPos);
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
Matrix4x4 WorldCamera::getInverseBufferCamera()
{
    Matrix4x4 inverse = this->bufferCamera;
    inverse.setInverse();

    return inverse;
}
void WorldCamera::setInverse() 
{
    this->worldCamera.setInverse();
}
