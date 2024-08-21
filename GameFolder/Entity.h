#pragma once
#include <tuple>
#include <string>
#include "Component.hpp"

typedef std::tuple< CTransform,
					CCollision,
					CScore,
					CLifespan,
					CInput,
					CAnimation,
					CBoundingBox,
					CGravity,
					CSprite
				  > ComponentTuple;

/*
Class representing an entity.
*/
class Entity
{
	//Object of class Entity can ONLY be created by an EntityManager type object
	friend class EntityManager;
private:
	const size_t      _id = 0;
	const std::string _tag = "Default";
	bool		      _is_alive = true;
	ComponentTuple	  _components;

	/*
	\param tag: name of the type of entity
	\param id: number of the entity (since EntityManager was created)
	*/
	Entity(const std::string& tag, size_t id);

public:

	/*
	Get the tag of the entity.
	\return const reference to the tag of the entity
	*/
	const std::string& tag() const;

	/*
	Get the id of the entity.
	\return const id of the entity
	*/
	const size_t id() const;

	/*
	Get the status of the entity (alive or not).
	\return 1 if alive, 0 if not
	*/
	const bool is_alive() const;

	/*
	Destroy the entity. Changes the 'alive' property to false.
	*/
	void destroy();

	/*
	Check if entity has component of type.
	\param <T> component type
	*/
	template<typename T>
	bool hasComponent()
	{
		return getComponent<T>().has;
	}
	
	/*
	Get a component of an entity.
	\param <T> component type
	\return reference to the component stored in the entity
	*/
	template<typename T>
	T& getComponent()
	{
		return std::get<T>(_components);
	}


	/*
	Add a component to the entity.
	\param <T> component type
	\param args: constructor parameters for T type component
	*/
	template<typename T, typename ...TArgs>
	T& addComponent(TArgs&&... args)
	{
		auto& c = getComponent<T>();
		c = T(std::forward<TArgs>(args)...);
		c.has = true;
		return c;
	};
};

