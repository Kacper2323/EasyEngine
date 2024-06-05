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
	sf::Text _pointTextBuffer;
	bool _showMenu = false;

public:

	PlayScene();
	PlayScene(GameEngine* game_ptr);
	void init();

	virtual void update() override;
	virtual void sDoAction(Action action) override;
	virtual void sRender() override;
	void sBBRender();
	void sMovement();
	void sCollision();
	void sPlayerMovement();
	void sLifespan();

	int _appleFrameCounter = 0;
	int _sawFrameCounter = 0;
	void sSawSpawner();
	void sAppleSpawner();
	void sViewSet();
	void readLevelCfgF(const std::string& path);
	void menu();
};