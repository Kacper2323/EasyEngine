 
#include "Entity.h"

Entity::Entity(const std::string& tag, size_t id)
	:_id(id), _tag(tag)
{};

const size_t Entity::id() const
{
	return _id;
}

const std::string& Entity::tag() const
{
	return _tag; 
};

const bool Entity::is_alive() const
{
	return _is_alive;
}

void Entity::destroy()
{
	_is_alive = false;
};




