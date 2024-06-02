#include "PlayScene.h"
#include "../Physics.h"

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
	registerAction(sf::Keyboard::L, "attack");
	registerAction(sf::Keyboard::LShift, "dash");

	_player = _entities.addEntity("knight");
	_player->addComponent<CTransform>(Vec2(100, 200), Vec2(0, 0), 0);
	_player->addComponent<CGravity>(0.85);
	_player->addComponent<CBoundingBox>(Vec2(80, 80));
	_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorIdle"));
	_player->addComponent<CInput>();

	auto e = _entities.addEntity("obstacle");
	e->addComponent<CTransform>(Vec2(300, 500), Vec2(0, 0), 0);
	e->addComponent<CBoundingBox>(Vec2(2000, 100));
	e->addComponent<CSprite>(_game->getAssets().getTexture("cloudFloor"), Vec2(500, 100));
}


void PlayScene::update()
{
	_entities.Update();
	sMovement();
	sCollision();
	sPlayerMovement();
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
		_player->getComponent<CTransform>().velocity.y = -20;
		_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorSwordJump"));
	}
	else if (action.name() == "attack" && action.type() == "START")
	{
		_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorAttack"));
		if (_playerState == PS_IDLE_L || _playerState == PS_LEFT_L || _playerState == PS_LEFT_LR)
		{
			_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
		}
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

			e->getComponent<CTransform>().velocity += e->getComponent<CTransform>().acceleration;
			e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;

			Vec2 eVel = e->getComponent<CTransform>().velocity;

			if (eVel.y > 18)
			{
				e->getComponent<CTransform>().velocity.y = 18;
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
}


void PlayScene::sRender()
{
	_game->window().clear();

	auto& p = _player->getComponent<CTransform>();
	//std::cout << p.pos.x << " : " << p.pos.y << std::endl;
	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{

		if (!e->getComponent<CTransform>().has) { continue; };

		
		if (e->getComponent<CAnimation>().has)
		{
			
			e->getComponent<CAnimation>().animation.getSprite().setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);

			e->getComponent<CAnimation>().animation.update();

			_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
		}
		else if (e->getComponent<CSprite>().has)
		{
			e->getComponent<CSprite>().sprite.setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);
			_game->window().draw(e->getComponent<CSprite>().sprite);
		}

	}
	sBBRender();

	_game->window().display();
}


void PlayScene::sPlayerMovement()
{
	auto& cInput = _player->getComponent<CInput>();
	auto& cAnim = _player->getComponent<CAnimation>();
	auto& cTrans = _player->getComponent<CTransform>();
	bool animCancelFlag = false;

	switch (_playerState)
	{
	case PS_IDLE_R:

		cTrans.velocity.x = 0.0f;

		animCancelFlag = cAnim.animation.getName() == "warriorRun";

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorIdle"));
		}

		animCancelFlag = false;

		if (cInput.right) { _playerState = PS_RIGHT_R; };
		if (cInput.left) { _playerState = PS_LEFT_L; };
		break;

	case PS_IDLE_L:

		cTrans.velocity.x = 0.0f;

		animCancelFlag = cAnim.animation.getName() == "warriorRun";

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorIdle"));
			_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
		}

		animCancelFlag = false;

		if (cInput.right) { _playerState = PS_RIGHT_R; };
		if (cInput.left) { _playerState = PS_LEFT_L; };
		break;

	case PS_RIGHT_R:

		animCancelFlag = cAnim.animation.getName() == "warriorIdle" || cTrans.velocity.x < 0;

		cTrans.velocity.x = _playerConfig.speed;

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorRun"));
		}

		animCancelFlag = false;

		if (!cInput.right) { _playerState = PS_IDLE_R; };
		if (cInput.left) { _playerState = PS_LEFT_LR; };
		break;

	case PS_LEFT_L:

		animCancelFlag = cAnim.animation.getName() == "warriorIdle" || cTrans.velocity.x > 0;

		cTrans.velocity.x = -_playerConfig.speed;

		if (cAnim.animation.hasEnded() || animCancelFlag)
		{
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorRun"));
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
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorRun"));
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
			_player->addComponent<CAnimation>(_game->getAssets().getAnimation("warriorRun"));
			_player->getComponent<CAnimation>().animation.getSprite().setScale(-1, 1);
		}

		animCancelFlag = false;

		if (!cInput.right) { _playerState = PS_LEFT_L; };
		if (!cInput.left) { _playerState = PS_RIGHT_R; }
		break;
	}

}