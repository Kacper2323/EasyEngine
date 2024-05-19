#pragma once

#include <string>


/*
Mappings for keys from different devices (mouse, keyboard) are represented
by the same number in enum form and, therefore, each device needs an offset.
It is used in scenes if they need to register actions as integers.
*/
enum actionMapOffsets
{
	mouseButtonOFFSET = 1000,
	mouseWheelOFFSET = 2000
};

/*
Class for actions relating to user input
*/
class Action
{
	std::string _name = "NONE";
	std::string _type = "NONE";
public:

	//mouse position at the moment of the action, if set
	int mouseX = 0;
	int mouseY = 0;

	//number of mouse wheel ticks at the moment of the action, if set
	//used pretty much only for mouse wheel action
	int mouseWheelDelta = 0;
	Action();

	/*
	\param name: name of the action
	\param type: type of the action "START" or "STOP";
	*/
	Action(const std::string& name, const std::string& type);
	
	/*
	Get the name of the action.
	\return name of the action
	*/
	const std::string& name() const;

	/*
	Get the type of the action.
	\return type of the action, "START" or "STOP"
	*/
	const std::string& type() const;
};
