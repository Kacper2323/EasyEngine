#pragma once

#include "../SceneABS.h"
#include "../GameEngine.h"
#include "../Component.hpp"
#include "../Animation.h"
#include "../Utils/messages.h"

struct PlayerConfig
{
	float speed = 4.0f;
};

enum PlayerState 
{
	PS_IDLE_R,
	PS_IDLE_L,
	PS_LEFT_L,
	PS_RIGHT_R,
	PS_LEFT_LR,
	PS_RIGHT_LR,
	PS_DASH_L,
	PS_DASH_R
};

class PlayScene : public Scene
{
	std::shared_ptr<Entity> _player;
	PlayerState _playerState = PS_IDLE_R;
	PlayerConfig	_playerConfig;
	sf::View _view;
	sf::Clock _deltaClock;
	sf::Text _pointTextBuffer;	//number of points the player has
	bool _showMenu = false;

public:

	PlayScene();
	PlayScene(GameEngine* game_ptr);

	/*
	Initialize the level.
	Constructs the player entity and map boundries.
	*/
	void init();

	/*
	Update entities and run systems.
	*/
	virtual void update() override;

	/*
	Do action.
	Resolves player input.
	*/
	virtual void sDoAction(Action action) override;

	/*
	Render system.
	Draws to the games' render window.
	*/
	virtual void sRender() override;

	/*
	Render bounding boxes of all entities.
	*/
	void sBBRender();

	/*
	Movement system.
	Specifies the movement of entities.
	*/
	void sMovement();

	/*
	Collision system.
	Resolves the collision of different types of entities.
	*/
	void sCollision();

	/*
	Player movement.
	Specifies the movement for the player.
	This function contains the state machine that controls
	the players movement and animations.
	*/
	void sPlayerMovement();

	/*
	Lifespan system.
	Resolves the end of lifespan for all entities that have
	the CLifespan component.
	*/
	void sLifespan();

	//counters to keep the last frame at which entity was spawned
	//cannot be made static in the functions as it would keep its value
	//between scene switches
	////////////////////////////
	int _appleFrameCounter = 0;
	int _sawFrameCounter = 0;
	////////////////////////////

	/*
	Saw spawner.
	Spawns rotating saws from left and right walls at random y
	locations.
	*/
	void sSawSpawner();

	/*
	Apple spawner.
	Spawn apples at radom platforms.
	Every apple has CScore component with 1 point.
	*/
	void sAppleSpawner();

	/*
	Set the view.
	Sets the game view to 1/4 of the game screen.
	*/
	void sViewSet();

	/*
	Read level config file.
	Reads and parses the level config file.
	Loads all the entities.
	*/
	void readLevelCfgF(const std::string& path);

	/*
	ImGui menu.
	*/
	void menu();
};