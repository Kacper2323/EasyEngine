#pragma once

#include "../SceneABS.h"
#include "../GameEngine.h"
#include "../Component.hpp"
#include "../Animation.h"

struct PlayerConfig
{
	float speed = 5.0f;
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

public:

	PlayScene();
	PlayScene(GameEngine* game_ptr);
	void init();

	void update();
	void sDoAction(Action action);
	void sRender();
	void sBBRender();
	void sMovement();
	void sCollision();
	void sPlayerMovement();
};