#include "GameEngine.h"
#include "Scenes/PlayScene/PlayScene.h"

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	InitWindow(1280, 720, "Game1");
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetTargetFPS(60);

	rlImGuiSetup(true);

	_assets.addTexture("Idle.png", "./GameFolder/Assets/Textures/Idle.png");
	_assets.addTexture("Run.png", "./GameFolder/Assets/Textures/Run.png");
	_assets.addTexture("Jump.png", "./GameFolder/Assets/Textures/Jump.png");
	_assets.addTexture("Fall.png", "./GameFolder/Assets/Textures/Fall.png");
	_assets.addTexture("saw38.png", "./GameFolder/Assets/Textures/saw38.png");
	_assets.addTexture("Apple.png", "./GameFolder/Assets/Textures/Apple.png");
	_assets.addTexture("hit.png", "./GameFolder/Assets/Textures/hit.png");

	_assets.addAnimation("frogIdle", "Idle.png", 11, 3, Vec2(0, 0), Vec2(32, 32));
	_assets.addAnimation("frogRun", "Run.png", 12, 3, Vec2(0, 0), Vec2(32, 32));
	_assets.addAnimation("frogJump", "Jump.png", 1, 2, Vec2(0, 0), Vec2(32, 32));
	_assets.addAnimation("frogFall", "Fall.png", 1, 2, Vec2(0, 0), Vec2(32, 32));
	_assets.addAnimation("saw", "saw38.png", 8, 2, Vec2(0, 0), Vec2(38, 38));
	_assets.addAnimation("Apple", "Apple.png", 17, 2, Vec2(0, 0), Vec2(32, 32));
	_assets.addAnimation("hit", "hit.png", 7, 2, Vec2(0, 0), Vec2(32, 32));

	_assets.addFont("OpenSans.ttf", "./GameFolder/zOpenSans.ttf");

	changeScene("PlayScene", std::make_shared<PlayScene>(this), 0);
}

void GameEngine::run()
{
	while (!WindowShouldClose())
	{
		update();

		sUserInput();

		currentScene()->sRender();

		if (_running == false)
		{
			quit();
		}
	}

	rlImGuiShutdown();
	CloseWindow();
}

void GameEngine::update()
{
	_scenes[_currentScene]->update();
}

void GameEngine::quit()
{

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


void GameEngine::sUserInput()
{

	for (auto [key, action] : currentScene()->getActionMap())
	{
		if (IsKeyPressed(key))
		{
			currentScene()->doAction(Action(action, "START"));
		}

		if (IsKeyReleased(key))
		{
			currentScene()->doAction(Action(action, "END"));
		}
	}
}