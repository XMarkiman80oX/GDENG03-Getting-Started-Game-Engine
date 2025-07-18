#pragma once
#include "SystemAttributes.h"

class IEntity
{
protected:
    EntityId id = 0;

public: // Constructors
    IEntity(EntityId id) : id(id) {}
    IEntity() : id(0) {}
    virtual ~IEntity() = default;

public:
    EntityId getId() const { return id; }
};