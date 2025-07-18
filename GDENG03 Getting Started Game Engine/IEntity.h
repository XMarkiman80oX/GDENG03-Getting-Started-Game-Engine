#pragma once
#include "SystemAttributes.h"

class IEntity
{
public:
    IEntity(EntityId id) : id(id) {}
    virtual ~IEntity() = default;
    EntityId getId() const { return id; }
private:
    EntityId id;
};