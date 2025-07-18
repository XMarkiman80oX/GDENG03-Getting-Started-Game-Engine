#pragma once
#include "BaseComponent.h"
#include "Prerequisites.h"

class RenderComponent : public BaseComponent
{
public:
    MeshPtr mesh;
    //MaterialPtr material;
    TexturePtr texture;
};