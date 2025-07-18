#pragma once
#include "SystemAttributes.h"

// IComponentArray is an interface that provides a way for the ComponentManager
// to notify a component array when an entity has been destroyed. This allows
// for proper cleanup of component data associated with the destroyed entity.
class IComponentArray
{
public:
    // Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~IComponentArray() = default;
    // Pure virtual function that must be implemented by derived classes.
    // This function is called when an entity is destroyed.
    virtual void entityDestroyed(EntityId entity) = 0;
};
