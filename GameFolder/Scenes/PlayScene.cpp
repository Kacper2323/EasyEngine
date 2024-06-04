#include "PlayScene.h"
#include "../Physics.h"
#include <fstream>
#include "./platformerLevelEdit.h"

PlayScene::PlayScene() {};

PlayScene::PlayScene(GameEngine* game_ptr)
	:Scene(game_ptr)
{
	init();
};


void PlayScene::init()
{
	registerAction(sf::Keyboard::A, "mvLeft");
	registerAction(sf::Keyboard::D, "mvRight");
	registerAction(sf::Keyboard::Space, "mvJump");
	registerAction(sf::Keyboard::LShift, "dash");
	registerAction(sf::Keyboard::Escape, "menu");

	_player = _entities.addEntity("frog");
	_player->addComponent<CTransform>(Vec2(100, 200), Vec2(0, 0), 0);
	_player->addComponent<CGravity>(0.85);
	_player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogIdle"));
	_player->addComponent<CBoundingBox>(Vec2(20, 24));
	_player->addComponent<CInput>();

	std::shared_ptr<Entity> e;

	//left wall
	for (int i = 24; i < 744; i += 48)
	{
		e = _entities.addEntity("terrain");
		e->addComponent<CTransform>(Vec2(8, i), Vec2(0, 0), 0);
		e->addComponent<CSprite>(_game->getAssets().getTexture("terrain.png"), sf::IntRect(240, 64, 16, 48));
	}
	//right wall
	for (int i = 24; i < 744; i += 48)
	{
		e = _entities.addEntity("terrain");
		e->addComponent<CTransform>(Vec2(1280-8, i), Vec2(0, 0), 0);
		e->addComponent<CSprite>(_game->getAssets().getTexture("terrain.png"), sf::IntRect(240, 64, 16, 48));
	}
	//ceiling
	for (int i = 40; i < 1280; i += 48)
	{
		e = _entities.addEntity("tarrain");
		e->addComponent<CTransform>(Vec2(i, 8), Vec2(0, 0), 0);
		e->addComponent<CSprite>(_game->getAssets().getTexture("terrain.png"), sf::IntRect(192, 64, 48, 16));
	}
	//floor
	for (int i = 40; i < 1280; i += 48)
	{
		e = _entities.addEntity("terrain");
		e->addComponent<CTransform>(Vec2(i, 720 - 24), Vec2(0, 0), 0);
		e->addComponent<CSprite>(_game->getAssets().getTexture("terrain.png"), sf::IntRect(96, 0, 48, 48));
	}
	
	e = _entities.addEntity("boundry");
	e->addComponent<CTransform>(Vec2(16/2, 720/2), Vec2(0,0), 0);
	e->addComponent<CBoundingBox>(Vec2(16, 720));

	e = _entities.addEntity("boundry");
	e->addComponent<CTransform>(Vec2(1280-8, 720/2), Vec2(0, 0), 0);
	e->addComponent<CBoundingBox>(Vec2(16, 720));

	e = _entities.addEntity("boundry");
	e->addComponent<CTransform>(Vec2(1280/2, 8), Vec2(0, 0), 0);
	e->addComponent<CBoundingBox>(Vec2(1280-16*2, 16));

	e = _entities.addEntity("boundry");
	e->addComponent<CTransform>(Vec2(1280/2, 720-24), Vec2(0, 0), 0);
	e->addComponent<CBoundingBox>(Vec2(1280-16*2, 48));

	readLevelCfgF("./cfgTemp.txt");
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
					sf::IntRect texRect;
					fin >> textureName >> texRect.left >> texRect.top >> texRect.width >> texRect.height;
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
	ImGui::SFML::Update(_game->window(), _deltaClock.restart());

	if (_showMenu)
	{
		menu();
	}

	if (_paused) { return; };

	_entities.Update();
	sMovement();
	sCollision();
	sPlayerMovement();
	sSawSpawner();

	_currentFrame++;
}

void PlayScene::menu()
{
	ImGui::Begin("Menu");

	ImGui::Text("Open level editor: ");

	if (ImGui::Button("Edit level"))
	{
		_game->changeScene("LevelEditor", std::make_shared<PLevelEditor>(_game), 1);
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

	sf::Color translucent(0, 0, 0, 0);
	sf::Color redOutline(255, 0, 0, 255);

	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{
		if (e->getComponent<CBoundingBox>().has)
		{
			Vec2 bbSize = e->getComponent<CBoundingBox>().size;
			Vec2 bbPos = e->getComponent<CTransform>().pos;

			sf::RectangleShape bb(sf::Vector2f(bbSize.x, bbSize.y));
			bb.setPosition(bbPos.x, bbPos.y);
			bb.setOrigin(bbSize.x / 2, bbSize.y / 2);
			bb.setFillColor(translucent);
			bb.setOutlineColor(redOutline);
			bb.setOutlineThickness(1);
			_game->window().draw(bb);
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


			if (eTransform.velocity.y > 10)
			{
				eTransform.velocity.y = 10;
			}

		}
	}
}


void PlayScene::sCollision()
{
	for (std::shared_ptr<Entity> e : _entities.getEntities("obstacle"))
	{
		Vec2 ov = Physics::getOverlap(e, _player);
		Vec2 prevOv = Physics::getPrevOverlap(e, _player);

		if (ov.y >= 0 && ov.x >=0)
		{
			_player->getComponent<CTransform>().pos.y = _player->getComponent<CTransform>().pos.y - ov.y;
		}
	}

	for (auto e : _entities.getEntities("boundry"))
	{
		Vec2 ov = Physics::getOverlap(e, _player);
		auto& pTransform = _player->getComponent<CTransform>();

		if (ov.y >= 0 && ov.x >= 0)
		{
			if (ov.y > ov.x)	//if there is more overlap on y, were moving along x
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
			else if (ov.y <= ov.x)
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
}

void PlayScene::sViewSet()
{
	sf::Vector2u windowSize = _game->window().getSize();
	_view.setSize(windowSize.x / 2, windowSize.y / 2);

	Vec2 pPos = _player->getComponent<CTransform>().pos;
	float viewCenterX = pPos.x;
	float viewCenterY = pPos.y;

	if (pPos.y - windowSize.y/4 < 0)
	{
		viewCenterY = windowSize.y / 4;
	}
	else if (pPos.y + windowSize.y/4 > windowSize.y)
	{
		viewCenterY = windowSize.y - windowSize.y / 4;
	}

	if (pPos.x - windowSize.x/ 4 < 0)
	{
		viewCenterX = windowSize.x / 4;
	}
	else if (pPos.x + windowSize.x / 4 > windowSize.x)
	{
		viewCenterX = windowSize.x - windowSize.x / 4;
	}
	
	_view.setCenter(viewCenterX, viewCenterY);

	_game->window().setView(_view);
}

void PlayScene::sRender()
{
	_game->window().clear(sf::Color(137, 207, 240, 255));

	//sViewSet();

	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{

		if (!e->getComponent<CTransform>().has) { continue; };

		if (e->getComponent<CAnimation>().has)
		{
			auto& eAnim = e->getComponent<CAnimation>();

			eAnim.animation.getSprite().setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);

			eAnim.animation.update();
			_game->window().draw(eAnim.animation.getSprite());
		}
		else if (e->getComponent<CSprite>().has)
		{
			e->getComponent<CSprite>().sprite.setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);
			_game->window().draw(e->getComponent<CSprite>().sprite);
		}

	}
	sBBRender();
	ImGui::SFML::Render(_game->window());
	_game->window().display();
}

void PlayScene::sSawSpawner()
{
	static int frame = _currentFrame;
	if (_currentFrame - frame < 60) { return; };

	frame = _currentFrame;
	auto e = _entities.addEntity("saw");

	int Ypos = 50 + (rand() % 600);
	int Xpos = Ypos%2 ? -50 : 1330;

	float Xvel = Ypos%2 ? 10 : -10;

	e->addComponent<CTransform>(Vec2(Xpos, Ypos), Vec2(Xvel, 0), 0);
	e->addComponent<CAnimation>(_game->getAssets().getAnimation("saw"));

}

void PlayScene::sPlayerMovement()
{
	auto& cInput = _player->getComponent<CInput>();
	auto& cAnim = _player->getComponent<CAnimation>();
	auto& cTrans = _player->getComponent<CTransform>();
	bool animCancelFlag = false;

	auto& pTransform = _player->getComponent<CTransform>();

	if (pTransform.pos.y != pTransform.prevPos.y)
	{
		if (pTransform.pos.y - pTransform.prevPos.y < 0)
		{
			cAnim = _player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogJump"));
			if (_playerState == PS_IDLE_L || _playerState == PS_LEFT_L || _playerState == PS_LEFT_LR)
			{
				cAnim.animation.getSprite().setScale(-1, 1);
			}
		}
		else
		{
			cAnim = _player->addComponent<CAnimation>(_game->getAssets().getAnimation("frogFall"));
			if (_playerState == PS_IDLE_L || _playerState == PS_LEFT_L || _playerState == PS_LEFT_LR)
			{
				cAnim.animation.getSprite().setScale(-1, 1);
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
			_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
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
			_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
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
			_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
		}

		animCancelFlag = false;

		if (!cInput.right) { _playerState = PS_LEFT_L; };
		if (!cInput.left) { _playerState = PS_RIGHT_R; }
		break;
	}

}