#include "EntityManager.h"

void EntityManager::removeDeadEntities()
{
	auto toRemove = [](std::shared_ptr<Entity> e) {return !e->_is_alive; };

	auto it = std::remove_if(_entities.begin(), _entities.end(), toRemove);
	_entities.erase(it, _entities.end());
	for (auto& v : _entityMap)
	{
		it = std::remove_if(v.second.begin(), v.second.end(), toRemove);
		v.second.erase(it, v.second.end());
	}
}


void EntityManager::Update()
{	
	for (std::shared_ptr<Entity> e : _toAdd)
	{
		_entities.push_back(e);
		_entityMap[e->tag()].push_back(e);
	}
	_toAdd.clear();

	removeDeadEntities();
}


std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(tag, _totalEntities++));
	_toAdd.push_back(e);
	return e;
}


EntityVec& EntityManager::getEntities()
{
	return _entities;
}


EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return _entityMap[tag];
}

const std::vector<std::string> EntityManager::getTags()
{
	std::vector<std::string> tags;

	for (const auto& [k, v] : _entityMap)
	{
		tags.push_back(k);
	}
	return tags;
}