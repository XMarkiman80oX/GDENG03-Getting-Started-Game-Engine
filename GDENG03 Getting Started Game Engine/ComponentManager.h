#pragma once
#include "SystemAttributes.h"
#include "IComponentArray.h"
#include <memory>
#include <unordered_map>
#include <array>

// A templated class that manages a contiguous array of components of a specific type.
// It inherits from IComponentArray to allow for type-erased storage and operations
// in the ComponentManager.
template<typename T>
class ComponentArray : public IComponentArray
{
public:
	// Inserts a component for a given entity.
	void insertData(EntityId entity, T component)
	{
		mComponentArray[entity] = component;
	}

	// Removes a component for a given entity by resetting it to its default state.
	void removeData(EntityId entity)
	{
		mComponentArray[entity] = T{};
	}

	// Retrieves a reference to the component for a given entity.
	T& getData(EntityId entity)
	{
		return mComponentArray[entity];
	}

	// Overrides the base class method to handle entity destruction.
	void entityDestroyed(EntityId entity) override
	{
		removeData(entity);
	}

private:
	std::array<T, MAX_ENTITIES> mComponentArray{};
};


// Manages all component types and their storage.
class ComponentManager
{
public:
	// Registers a new component type with the manager.
	template<typename T>
	void registerComponent()
	{
		const char* typeName = typeid(T).name();
		componentTypes.insert({ typeName, nextComponentType });
		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });
		nextComponentType++;
	}

	// Retrieves the unique ID for a component type.
	template<typename T>
	ComponentId getComponentType()
	{
		const char* typeName = typeid(T).name();
		return componentTypes[typeName];
	}

	// Adds a component to an entity.
	template<typename T>
	void addComponent(EntityId entity, T component)
	{
		getComponentArray<T>()->insertData(entity, component);
	}

	// Removes a component from an entity.
	template<typename T>
	void removeComponent(EntityId entity)
	{
		getComponentArray<T>()->removeData(entity);
	}

	// Retrieves a component for a given entity.
	template<typename T>
	T& getComponent(EntityId entity)
	{
		return getComponentArray<T>()->getData(entity);
	}

	// Notifies all component arrays that an entity has been destroyed.
	void entityDestroyed(EntityId entity)
	{
		for (auto const& pair : componentArrays)
		{
			auto const& component = pair.second;
			component->entityDestroyed(entity);
		}
	}

private:
	std::unordered_map<const char*, ComponentId> componentTypes{};
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays{};
	ComponentId nextComponentType{};

	// Helper function to get the specific component array for a type.
	template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
		const char* typeName = typeid(T).name();
		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}
};
