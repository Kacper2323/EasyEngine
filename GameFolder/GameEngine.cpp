#include "GameEngine.h"
#include "Scenes/PlayScene.h"
#include "Scenes/platformerLevelEdit.h"

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	_window.create(sf::VideoMode(1280, 720), "GAME1", sf::Style::Titlebar | sf::Style::Close);

	_window.setFramerateLimit(60);
	//_window.setVerticalSyncEnabled(true);

	ImGui::SFML::Init(_window);

	_assets.addTexture("warriorBlue", "./GameFolder/Warrior_Blue.png");
	_assets.addTexture("cloudFloor", "./GameFolder/cloud.png");
	_assets.addAnimation("warriorIdle", "warriorBlue", 6, 4, Vec2(0, 0 * 192), Vec2(192, 192));
	_assets.addAnimation("warriorRun", "warriorBlue", 6, 4, Vec2(0, 1*192), Vec2(192, 192));
	_assets.addAnimation("warriorAttack", "warriorBlue", 6, 3, Vec2(0, 2*192), Vec2(192, 192));
	_assets.addAnimation("warriorSwordJump", "warriorBlue", 6, 3, Vec2(0, 5*192), Vec2(192, 192));
	_assets.addAnimation("warriorDash", "warriorBlue", 6, 3, Vec2(3, 0 * 192), Vec2(192, 192));

	changeScene("levelEdit", std::make_shared<PLevelEditor>(this), 1);
}

void GameEngine::run()
{
	while (_running)
	{
		//ImGui::SFML::Update(_window, _deltaClock.restart());

		update();

		sUserInput();

		currentScene()->sRender();

		if (_running == false)
		{
			quit();
		}
	}
	ImGui::SFML::Shutdown();
}

void GameEngine::update()
{
	_scenes[_currentScene]->update();
}

void GameEngine::quit()
{
	_window.close();
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrent)
{
	if (endCurrent)
	{
		_scenes.erase(_currentScene);
	}

	_currentScene = sceneName;

	if (_scenes.find(sceneName) == _scenes.end())
	{
		_scenes[sceneName] = scene;
	}
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return _scenes[_currentScene];
}

Assets& GameEngine::getAssets()
{
	return _assets;
}

sf::RenderWindow& GameEngine::window()
{
	return _window;
}

void GameEngine::sUserInput()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		//ImGui::SFML::ProcessEvent(event);
		ImGui::SFML::ProcessEvent(event);
		if (event.type == sf::Event::Closed)
		{
			_running = false;
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) 
			{ continue; }

			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}

		if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
		{
			if (currentScene()->getActionMap().find(event.key.code + mouseButtonOFFSET) == currentScene()->getActionMap().end())
			{
				continue;
			}

			const std::string actionType = (event.type == sf::Event::MouseButtonPressed) ? "START" : "END";

			Action action(Action(currentScene()->getActionMap().at(event.mouseButton.button + mouseButtonOFFSET), actionType));

			action.mouseX = event.mouseButton.x;
			action.mouseY = event.mouseButton.y;

			currentScene()->doAction(action);
		}
		
		if (event.type == sf::Event::MouseWheelMoved)
		{
			if (currentScene()->getActionMap().find(mouseWheelOFFSET) == currentScene()->getActionMap().end())
			{
				continue;
			}

			Action action(Action(currentScene()->getActionMap().at(mouseWheelOFFSET), "START"));

			action.mouseX = event.mouseWheel.x;
			action.mouseY = event.mouseWheel.y;
			action.mouseWheelDelta = event.mouseWheel.delta;


			currentScene()->doAction(action);
		}
	}
}