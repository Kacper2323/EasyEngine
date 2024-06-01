#pragma once

#include "Action.h"
#include "EntityManager.h"

class GameEngine;

typedef std::map<int, std::string> ActionMap;

/*
Abstract class to create scenes by inheritance.
Contains a method for registering actions, an EntityManager object and an action map.
Requires an implementation of update(), doAction(Action) and sRender() methods to inherit.
Requires a game engine pointer to have access to global assets and the scene switching method.
*/
class Scene
{

protected:
	GameEngine* _game = nullptr;
	EntityManager _entities;
	ActionMap _actionMap;
	int _currentFrame = 0;
	bool _paused = false;
	bool _hasEnded = false;

	//virtual void onEnd() = 0;
	void setPaused(bool paused);

public:

	Scene();
	Scene(GameEngine* game_ptr);

	/*
	Update function for all the systems, UI etc.
	Scene dependant.
	Will be called on every frame.
	*/
	virtual void update() = 0;

	/*
	Handling user input. Scene dependant, actions need to be registered with registerAction().
	\param action: action to be done
	*/
	virtual void sDoAction(Action action) = 0;

	/*
	Rendering system.
	Scene dependant.
	*/
	virtual void sRender() = 0;

	/*
	Update a number of times before the next frame.
	\param count: number of updates to do
	*/
	void simulate(int count);

	/*
	Call the sDoAction system.
	/param action: action to be done
	*/
	void doAction(Action action);

	/*
	Register a new action. Places a new [key: actionName] into the _actionMap which will be used by the GameEngine 
	to generate actions on input.
	\param key: integer key representing an input 
	(e.g. sf::Keyboard::S or sf::Mouse::Right + mouseButtonOffset (check Action header for more info on offsets))
	\param actionName: name of the action
	*/
	void registerAction(int key, const std::string& actionName);

	size_t currentFrame() const;

	bool hasEnded() const;
	const ActionMap& getActionMap() const;
	void drawLine(const Vec2& p1, const Vec2& p2);	//does nothing for now
};