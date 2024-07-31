#include "GameEngine.h"
#include "Scenes/PlayScene/PlayScene.h"
#include "Scenes/levelEditorScene/platformerLevelEdit.h"

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	_window.create(sf::VideoMode(1280, 720), "GAME1", sf::Style::Titlebar | sf::Style::Close);
	_window.setFramerateLimit(60);

	ImGui::SFML::Init(_window);

	_assets.addTexture("Idle.png", "./GameFolder/Assets/Textures/Idle.png");
	_assets.addTexture("Run.png", "./GameFolder/Assets/Textures/Run.png");
	_assets.addTexture("Jump.png", "./GameFolder/Assets/Textures/Jump.png");
	_assets.addTexture("Fall.png", "./GameFolder/Assets/Textures/Fall.png");
	_assets.addTexture("saw38.png", "./GameFolder/Assets/Textures/saw38.png");
	_assets.addTexture("Apple.png", "./GameFolder/Assets/Textures/Apple.png");
	_assets.addTexture("hit.png", "./GameFolder/Assets/Textures/hit.png");

	_assets.addAnimation("frogIdle", "Idle.png", 11, 3, Vec2(0, 0), Vec2(32, 40));
	_assets.addAnimation("frogRun", "Run.png", 12, 3, Vec2(0, 0), Vec2(32, 40));
	_assets.addAnimation("frogJump", "Jump.png", 1, 2, Vec2(0, 0), Vec2(32, 40));
	_assets.addAnimation("frogFall", "Fall.png", 1, 2, Vec2(0, 0), Vec2(32, 40));
	_assets.addAnimation("saw", "saw38.png", 8, 2, Vec2(0, 0), Vec2(38, 38));
	_assets.addAnimation("Apple", "Apple.png", 17, 2, Vec2(0, 0), Vec2(32, 32));
	_assets.addAnimation("hit", "hit.png", 7, 2, Vec2(0, 0), Vec2(32, 32));

	_assets.addFont("OpenSans.ttf", "./GameFolder/zOpenSans.ttf");

	changeScene("PlayScene", std::make_shared<PlayScene>(this), 0);
}

void GameEngine::run()
{
	while (_running)
	{
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