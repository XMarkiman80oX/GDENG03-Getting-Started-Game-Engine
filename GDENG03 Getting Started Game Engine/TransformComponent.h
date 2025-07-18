#pragma once
#include "BaseComponent.h"
#include "Vector3D.h"

class TransformComponent : public BaseComponent
{
public:
    Vector3D position;
    Vector3D rotation;
    Vector3D scale;
};