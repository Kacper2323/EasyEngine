#pragma once

#include <map>

#include "imgui.h"
#include "rlImGui.h"
#include "SceneABS.h"
#include "Assets.h"

//a map from names of scenes to scenes
typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

/*
Game Engine class.
Contains mechanisms for switching scenes, managing global assets 
as well as a user input system and the main game loop.
*/
class GameEngine
{
	SceneMap						_scenes;
	std::string						_currentScene;
	Assets							_assets;
	bool							_running = true;

public:



	GameEngine(const std::string& path);

	/*
	Game engine initialization. Creates and sets up a window.
	Global assets should be loaded here.
	\param path: path to a (.txt) configuration file (not yet implemented)
	*/
	void init(const std::string& path);

	/*
	Run the game loop.
	*/
	void run();

	/*
	Call the current scenes Update() function.
	*/
	void update();

	/*
	Quit the game. For now only closes the window.
	*/
	void quit();

	/*
	Change the current scene to another one. Will look for an existing scene with a given name,
	if it is found, it will change to that scene, if not, a new one will be added.

	\param sceneName: name of the scene to change to
	\param scene: shared pointer to a Scene object
	\param endCurrent: if 1 will delete the current scene before switching
	*/
	void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrent);

	/*
	Get the current scene object.
	\return shared pointer to the current scene
	*/
	std::shared_ptr<Scene> currentScene();

	/*
	Get the assets/
	\return reference to Asset type object containing global game assets
	*/
	Assets& getAssets();

	/*
	A user input system.
	Should be called on each frame. Will process user input and pass it to the current scene.
	Uses an actionMap contained within a scene to generate actions. If an action is registered within a scene,
	this function will create an Action type object and send it to the doAction method in the same scene to be executed.
	*/
	void sUserInput();
};