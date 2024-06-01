#pragma once

#include <iostream>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec>	 EntityMap;

/*
EntityManager class.
Can create and remove entities.
Holds a vector of all entities as well as a [tag: entity] map
It is the ONLY object that can create new entities.
*/
class EntityManager
{
private:
	EntityVec _entities;
	EntityVec _toAdd;
	EntityMap _entityMap;
	size_t	  _totalEntities = 0;

	void removeDeadEntities();

public:

	EntityManager() {};

	/*
	Update the entity manager.
	Adds entities created with addEntity to the game
	(addEntity ads it to a _toAdd vector, Update() must be called to add it to the game).
	Removes all entities which is_alive flag is set to false.
	*/
	void Update();

	/*
	Creates an entity (adds it to a _toAdd vector, Update() needs to be called to add an entity to the game)
	\param tag: tag to give to the entity
	\return shared pointer to the entity
	*/
	std::shared_ptr<Entity> addEntity(const std::string& tag);

	/*
	Get all entities.
	\return vector of shared pointers to all entities
	*/
	EntityVec& getEntities();

	/*
	Get entities with a certain tag.
	/param tag: tag of the entities
	\return vector of shared pointers to entities with a tag
	*/
	EntityVec& getEntities(const std::string& tag);


	/*
	Get all tags in the EntityMap.
	\return vector containing all the tags
	*/
	const std::vector<std::string> getTags();
};