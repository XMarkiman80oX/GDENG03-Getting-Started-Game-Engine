#pragma once
#include "IComponent.h"
#include "SystemAttributes.h"

class BaseComponent : public IComponent
{
public:
    static ComponentId nextId;
};