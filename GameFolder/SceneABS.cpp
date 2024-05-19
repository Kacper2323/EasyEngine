#include "SceneABS.h"


Scene::Scene() {}

Scene::Scene(GameEngine* game_ptr)
	:_game(game_ptr)
{}


void Scene::setPaused(bool paused)
{
	_paused = paused;
}

void Scene::simulate(int frames)
{
	for (int i = 0; i < frames; i++)
	{
		this->update();
	}
}

void Scene::doAction(Action action)
{
	this->sDoAction(action);
}

void Scene::registerAction(int key, const std::string& actionName)
{
	_actionMap[key] = actionName;
}

size_t Scene::currentFrame() const
{
	return _currentFrame;
}

bool Scene::hasEnded() const
{
	return _hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
	return _actionMap;
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2)
{

}