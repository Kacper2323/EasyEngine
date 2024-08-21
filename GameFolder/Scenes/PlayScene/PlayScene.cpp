#include "PlayScene.h"
#include "../../Physics.h"
#include <fstream>

PlayScene::PlayScene() {};

PlayScene::PlayScene(GameEngine* game_ptr)
	:Scene(game_ptr)
{
	init();
};


void PlayScene::init()
{
	SetExitKey(KEY_NULL);
	registerAction(KEY_A, "mvLeft");
	registerAction(KEY_D, "mvRight");
	registerAction(KEY_SPACE, "mvJump");
	registerAction(KEY_ESCAPE, "menu");

	_player = _entities.addEntity("frog");
	_player->addComponent<CTransform>(Vec2(600, 600), Vec2(0, 0), 0);
	_player->addComponent<CGravity>(0.85);
	_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogIdle"));
	_player->addComponent<CBoundingBox>(Vec2(20, 32));
	_player->addComponent<CInput>();
	_player->addComponent<CScore>(0);

	readLevelCfgF("./cfgTemp.cfg");
}


void PlayScene::readLevelCfgF(const std::string& path)
{
	std::fstream fin(path);
	std::string keyword;
	std::string component;

	while (fin >> keyword)
	{
		if (keyword == "ENTITY")
		{
			std::string entityTag;
			fin >> entityTag;
			auto entity = _entities.addEntity(entityTag);

			while (fin >> component)
			{
				if (component == "CBoundingBox")
				{
					Vec2 size;
					fin >> size.x >> size.y;
					entity->addComponent<CBoundingBox>(size);
				}
				else if (component == "CTransform")
				{
					Vec2 pos;
					Vec2 speed;
					float angle;
					fin >> pos.x >> pos.y >> speed.x >> speed.y >> angle;
					entity->addComponent<CTransform>(pos, speed, angle);
				}
				else if (component == "CSprite")
				{
					std::string textureName;
					Rectangle texRect;
					fin >> textureName;

					//if texture for a sprite is not loaded into the memory, load it
					if (_game->getAssets().getTextures().find(textureName) == _game->getAssets().getTextures().end())
					{
						_game->getAssets().addTexture(textureName, "./GameFolder/Assets/Textures/" + textureName);
					}
					
					fin >> texRect.x >> texRect.y >> texRect.width >> texRect.height;
					entity->addComponent<CSprite>(_game->getAssets().getTexture(textureName), texRect);
				}
				else if (component == "END")
				{
					break;
				}
				else
				{
					MSG::ERROR("Error in PlayScene cfg file: unknown keyword - ", component);
				}
			}
		}
	}
	fin.close();
}



void PlayScene::update()
{
	if (_paused) { return; };

	_entities.Update();
	sLifespan();
	sMovement();
	sCollision();
	sPlayerMovement();
	sSawSpawner();
	sAppleSpawner();
	_pointTextBuffer = _player->getComponent<CScore>().score;

	_currentFrame++;
}

void PlayScene::menu()
{
	ImGui::Begin("Menu");

	ImGui::Text("Open level editor: ");

	if (ImGui::Button("Edit level"))
	{
		//_game->changeScene("LevelEditor", std::make_shared<PLevelEditor>(_game), 1);
	}

	ImGui::End();
}

void PlayScene::sDoAction(Action action)
{
	auto& cInput = _player->getComponent<CInput>();
	static bool leftFlag = 0;
	static bool rightFlag = 0;

	if (action.name() == "mvRight" && action.type() == "START")
	{
		cInput.right = true;
	}
	else if (action.name() == "mvRight" && action.type() == "END")
	{
		cInput.right = false;
	}
	else if (action.name() == "mvLeft" && action.type() == "START")
	{
		cInput.left = true;
	}
	else if (action.name() == "mvLeft" && action.type() == "END")
	{
		cInput.left = false;
	}
	else if (action.name() == "dash" && action.type() == "START")
	{
		cInput.LShift = true;
	}
	else if (action.name() == "mvJump" && action.type() == "START")
	{
		_player->getComponent<CTransform>().velocity.y = -15;
	}
	else if (action.name() == "menu" && action.type() == "START")
	{
		_showMenu = _showMenu ? false : true;
		_paused = _showMenu;
	}
}


void PlayScene::sBBRender()
{
	//Must be called between window.clear() and window.display() in a routine

	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{
		if (e->getComponent<CBoundingBox>().has)
		{
			Vec2 bbSize = e->getComponent<CBoundingBox>().size;
			Vec2 bbPos = e->getComponent<CTransform>().pos;

			DrawRectangleLines(bbPos.x - bbSize.x/2, bbPos.y - bbSize.y/2, bbSize.x, bbSize.y, RED);
		}
	}
}

void PlayScene::sMovement()
{
	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{
		if (e->getComponent<CTransform>().has)
		{
			if (e->getComponent<CGravity>().has)
			{
				e->getComponent<CTransform>().acceleration.y = e->getComponent<CGravity>().gravAcc;
			}
			
			auto& eTransform = e->getComponent<CTransform>();

			eTransform.prevPos = eTransform.pos;

			eTransform.velocity += eTransform.acceleration;
			eTransform.pos += eTransform.velocity;


			if (eTransform.velocity.y > 7)
			{
				eTransform.velocity.y = 7;
			}

		}
	}
}


void PlayScene::sCollision()
{

	for (auto e : _entities.getEntities("terrain"))
	{
		Vec2 ov = Physics::getOverlap(e, _player);
		Vec2 prevOv = Physics::getPrevOverlap(e, _player);
		auto& pTransform = _player->getComponent<CTransform>();

		if (ov.y >= 0 && ov.x >= 0)
		{
			
			if (prevOv.x > 0)
			{
				if (pTransform.pos.y > pTransform.prevPos.y)
				{
					pTransform.pos.y -= ov.y;
				}
				else
				{
					pTransform.pos.y += ov.y;
				}
			}
			else if (prevOv.y > 0)	//if there is more overlap on y, were moving along x
			{
				if (pTransform.pos.x > pTransform.prevPos.x)
				{
					pTransform.pos.x -= ov.x;
				}
				else
				{
					pTransform.pos.x += ov.x;
				}
			}
		}
	}

	for (auto e : _entities.getEntities("platform"))
	{
		Vec2 ov = Physics::getOverlap(e, _player);
		Vec2 prevOv = Physics::getPrevOverlap(e, _player);
		auto& pTransform = _player->getComponent<CTransform>();

		if (ov.y >= 0 && ov.x >= 0)
		{
			if (prevOv.y > 0)	//if there is more overlap on y, were moving along x
			{
				if (pTransform.pos.x > pTransform.prevPos.x)
				{
					pTransform.pos.x -= ov.x;
				}
				else
				{
					pTransform.pos.x += ov.x;
				}
			}
			else if (prevOv.x > 0)
			{
				if (pTransform.pos.y > pTransform.prevPos.y)
				{
					pTransform.pos.y -= ov.y;
				}
				else
				{
					pTransform.pos.y += ov.y;
				}
			}
		}
	}

	for (std::shared_ptr<Entity> e : _entities.getEntities("saw"))
	{
		Vec2 ov = Physics::getOverlap(e, _player);

		if (ov.y > 0 && ov.x > 0)
		{
			e->destroy();
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("hit"));
			_player->getComponent<CScore>().score = 0;
			_showMenu = true;
			_paused = _showMenu;
		}
	}
	if (_player->getComponent<CAnimation>().animation.hasEnded() && _player->getComponent<CAnimation>().animation.getName() == "hit")
	{
		_player->getComponent<CTransform>().pos = Vec2(600, 600);
		
		for (auto e : _entities.getEntities("saw"))
		{
			e->destroy();
		}
	}



	for (std::shared_ptr<Entity> e : _entities.getEntities("Apple"))
	{
		Vec2 ov = Physics::getOverlap(e, _player);

		if (ov.y > 0 && ov.x > 0)
		{
			e->destroy();
			_player->getComponent<CScore>().score++;
		}
	}
}

void PlayScene::sViewSet()
{
	_camera.zoom = 2;
	_camera.offset = { 0,0 };
	Vec2 pPos = _player->getComponent<CTransform>().pos;
	_camera.target = { pPos.x, pPos.y };
	_camera.rotation = 0;


	float viewCenterX = pPos.x;
	float viewCenterY = pPos.y;

	if (pPos.y - GetScreenHeight()/4.0 < 0)
	{
		viewCenterY = GetScreenHeight() / 4.0;
	}
	else if (pPos.y + GetScreenHeight()/4.0 > GetScreenHeight())
	{
		viewCenterY = GetScreenHeight() - GetScreenHeight() / 4.0;
	}

	if (pPos.x - GetScreenWidth()/ 4 < 0)
	{
		viewCenterX = GetScreenWidth() / 4.0;
	}
	else if (pPos.x + GetScreenWidth() / 4 > GetScreenWidth())
	{
		viewCenterX = GetScreenWidth() - GetScreenWidth() / 4.0;
	}

	_camera.offset.x = viewCenterX;
	_camera.offset.y = viewCenterY;
}

void PlayScene::sRender()
{

	Color color = { 137, 207, 240, 255 };

	
	BeginDrawing();
	ClearBackground(color);

	sViewSet();

	BeginMode2D(_camera);

	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{

		if (!e->getComponent<CTransform>().has) { continue; };

		if (e->getComponent<CAnimation>().has)
		{
			auto& eAnim = e->getComponent<CAnimation>();

			Rectangle position = eAnim.animation.getSprite().textureRect;
			position.x = e->getComponent<CTransform>().pos.x;
			position.y = e->getComponent<CTransform>().pos.y;

			eAnim.animation.update();

			Sprite sprite = eAnim.animation.getSprite();

			if (eAnim.animation.horizontalFlip)
			{
				sprite.textureRect.width *= -1;
			}

			DrawTexturePro(sprite.texture, sprite.textureRect, position, sprite.origin, 0, RAYWHITE);
		}
		else if (e->getComponent<CSprite>().has)
		{

			Rectangle position = e->getComponent<CSprite>().textureRect;
			position.x = e->getComponent<CTransform>().pos.x;
			position.y = e->getComponent<CTransform>().pos.y;

			DrawTexturePro(e->getComponent<CSprite>().texture, e->getComponent<CSprite>().textureRect, position,
				e->getComponent<CSprite>().origin, 0, RAYWHITE);
		}

	}
	sBBRender();
	
	EndMode2D();

	rlImGuiBegin();

	if (_showMenu)
	{
		menu();
	}

	rlImGuiEnd();

	EndDrawing();
}

void PlayScene::sSawSpawner()
{
	if (_currentFrame - _sawFrameCounter < 20) { return; };

	_sawFrameCounter = _currentFrame;
	auto e = _entities.addEntity("saw");

	int Ypos = 50 + (rand() % 600);
	int Xpos = Ypos%2 ? -50 : 1330;

	float Xvel = Ypos%2 ? 6 : -6;

	e->addComponent<CTransform>(Vec2(Xpos, Ypos), Vec2(Xvel, 0), 0);
	e->addComponent<CAnimation>(_game->getAssets().getAnimation("saw"));
	e->addComponent<CBoundingBox>(Vec2(30, 30));
	e->addComponent<CLifespan>(240);
}

void PlayScene::sAppleSpawner()
{
	if (_currentFrame - _appleFrameCounter < 120) { return; };

	_appleFrameCounter = _currentFrame;

	EntityVec& platforms = _entities.getEntities("platform");

	int platform = (rand() % platforms.size());

	std::shared_ptr<Entity> randomPlatform = platforms[platform];
	auto platformPos = randomPlatform->getComponent<CTransform>().pos;

	auto e = _entities.addEntity("Apple");

	static int appleSize = 20;

	e->addComponent<CTransform>(Vec2(platformPos.x, platformPos.y-appleSize), Vec2(0, 0), 0);
	e->addComponent<CBoundingBox>(Vec2(appleSize, appleSize));
	e->addComponent<CScore>(1);
	e->addComponent<CAnimation>(_game->getAssets().getAnimation("Apple"));
	e->addComponent<CLifespan>(300);
}

void PlayScene::sLifespan()
{
	for (auto e : _entities.getEntities())
	{
		if (e->getComponent<CLifespan>().has)
		{
			e->getComponent<CLifespan>().remaining--;
			if (e->getComponent<CLifespan>().remaining <= 0)
			{
				e->destroy();
			}
		}
	}
}

void PlayScene::sPlayerMovement()
{
	auto& cInput = _player->getComponent<CInput>();
	auto& cAnim = _player->getComponent<CAnimation>();
	auto& cTrans = _player->getComponent<CTransform>();
	bool animCancelFlag = false;

	auto& pTransform = _player->getComponent<CTransform>();

	if (pTransform.pos.y != pTransform.prevPos.y && _player->getComponent<CAnimation>().animation.getName() != "hit")
	{
		if (pTransform.pos.y - pTransform.prevPos.y < 0)
		{
			cAnim = _player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogJump"));
			if (_playerState == PS_IDLE_L || _playerState == PS_LEFT_L || _playerState == PS_LEFT_LR)
			{
				//cAnim.animation.getSprite().textureRect.width *= -1;
				_player->getComponent<CAnimation>().animation.horizontalFlip = 1;
			}
		}
		else
		{
			cAnim = _player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogFall"));
			if (_playerState == PS_IDLE_L || _playerState == PS_LEFT_L || _playerState == PS_LEFT_LR)
			{
				//cAnim.animation.getSprite().setScale(-1, 1);
				_player->getComponent<CAnimation>().animation.horizontalFlip = 1;
			}
		}
	}

	switch (_playerState)
	{
	case PS_IDLE_R:

		cTrans.velocity.x = 0.0f;

		animCancelFlag = cAnim.animation.getName() == "frogRun";

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogIdle"));
		}

		animCancelFlag = false;

		if (cInput.right) { _playerState = PS_RIGHT_R; };
		if (cInput.left) { _playerState = PS_LEFT_L; };
		break;

	case PS_IDLE_L:

		cTrans.velocity.x = 0.0f;

		animCancelFlag = cAnim.animation.getName() == "frogRun";

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogIdle"));
			//_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
			_player->getComponent<CAnimation>().animation.horizontalFlip = 1;
		}

		animCancelFlag = false;

		if (cInput.right) { _playerState = PS_RIGHT_R; };
		if (cInput.left) { _playerState = PS_LEFT_L; };
		break;

	case PS_RIGHT_R:

		animCancelFlag = cAnim.animation.getName() == "frogIdle" || cTrans.velocity.x < 0;

		cTrans.velocity.x = _playerConfig.speed;

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogRun"));
		}

		animCancelFlag = false;

		if (!cInput.right) { _playerState = PS_IDLE_R; };
		if (cInput.left) { _playerState = PS_LEFT_LR; };
		break;

	case PS_LEFT_L:

		animCancelFlag = cAnim.animation.getName() == "frogIdle" || cTrans.velocity.x > 0;

		cTrans.velocity.x = -_playerConfig.speed;

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogRun"));
			//_player->getComponent<CAnimation>().animation.getSprite().textureRect.width *= -1;
			_player->getComponent<CAnimation>().animation.horizontalFlip = 1;
		}

		animCancelFlag = false;

		if (!cInput.left) { _playerState = PS_IDLE_L; };
		if (cInput.right) { _playerState = PS_RIGHT_LR; };
		break;

	case PS_RIGHT_LR:

		animCancelFlag = cTrans.velocity.x < 0;

		cTrans.velocity.x = _playerConfig.speed;

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogRun"));
		}

		animCancelFlag = false;

		if (!cInput.left) { _playerState = PS_RIGHT_R; };
		if(!cInput.right) { _playerState = PS_LEFT_L; }
		break;

	case PS_LEFT_LR:

		animCancelFlag = cTrans.velocity.x > 0;

		cTrans.velocity.x = -_playerConfig.speed;

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogRun"));
			//_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
			_player->getComponent<CAnimation>().animation.horizontalFlip = 1;
		}

		animCancelFlag = false;

		if (!cInput.right) { _playerState = PS_LEFT_L; };
		if (!cInput.left) { _playerState = PS_RIGHT_R; }
		break;
	}

}