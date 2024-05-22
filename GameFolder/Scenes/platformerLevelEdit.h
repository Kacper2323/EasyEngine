#pragma once
#include "../GameEngine.h"
#include "../SceneABS.h"
#include <filesystem>
#include "../Utils/messages.h"
#include "./LevelEditorTools/textureEditWindow.h"

struct textureData
{
	std::string texturePath;
	std::vector <std::string> textureNames;
};

struct ImGuiBinds
{
	std::string* current_obj = nullptr;
	sf::Color background = sf::Color::Black;
	bool showTextureWindow = false;
};


class PLevelEditor : public Scene
{
	//implement a circular queue for ctrl-z ctrl-y functions

public:
	ImGuiBinds imGuiBinds;
	textureData textureData;
	Assets editorAssets;
	sf::Clock deltaClock;
	TextureEditWindow editTexture;

	

	PLevelEditor();
	PLevelEditor(GameEngine* gameEnginePointer);


	void update();
	void sDoAction(Action action);
	void sRender();

	void init();
	void saveLevel();

	/*
	Toggle a grid on the window.
	\param gridSize: size of a square in the grid, in pixels
	\param alignBottomLeft: 1 to align the grid to bottom left corner, 0 if top left
	*/
	void gridToggle(int gridSize, bool alignBottomLeft);
	void multiplyEntity();

	/*
	Render the ImGUI interface.
	*/
	void ImGuiRender();
	void sBBRender();

	/*
	Open a texture view.
	*/
	void textureWindow();

	//test

	void test();
};
