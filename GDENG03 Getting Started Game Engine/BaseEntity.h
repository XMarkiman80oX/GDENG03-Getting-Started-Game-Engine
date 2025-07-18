#pragma once
#include "IEntity.h"

class BaseEntity : public IEntity
{
public:
    BaseEntity(EntityId id) : IEntity(id) {}
};