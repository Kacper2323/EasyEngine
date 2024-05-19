#include "Action.h"


Action::Action() {};

Action::Action(const std::string& name, const std::string& type)
	:_name(name), _type(type)
{};


const std::string& Action::name() const
{
	return _name;
};

const std::string& Action::type() const
{
	return _type;
};