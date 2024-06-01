#pragma once
#include "../GameEngine.h"
#include "../SceneABS.h"
#include <filesystem>
#include "../Utils/messages.h"
#include "./LevelEditorTools/textureEditWindow.h"

struct TextureData
{
	std::string texturePath;
	std::vector <std::string> textureNames;
};

struct ImGuiVars
{
	
	std::string* currentTexture = nullptr;
	std::string currentSprite;

	sf::Color background = sf::Color::Black;
	ImVec4 warningColor = { 1.0f, 1.0f, 0.0f, 1.0f };
	ImVec4 messageColor = { 0.0f, 1.0f, 1.0f, 1.0f };

	bool showTextureWindow = false;
	bool showEntityCreationWindow = false;

	//for the entity creation window
	std::shared_ptr<Entity> entity;
};

typedef std::map<std::string, sf::Sprite> Sprites;

class PLevelEditor : public Scene
{
	//implement a circular queue for ctrl-z ctrl-y functions
	ImGuiVars _imGuiVars;
	TextureData _textureData;
	sf::Clock _deltaClock;
	TextureEditWindow _textureWindow;
	Assets _editorAssets;


	Sprites _sprites;
	sf::Sprite _selectedSprite;
	void saveSprite(sf::Sprite& sprite);

public:

	PLevelEditor();
	PLevelEditor(GameEngine* gameEnginePointer);


	virtual void update() override;
	virtual void sDoAction(Action action) override;
	virtual void sRender() override;

	void init();
	void saveLevel();

	/*
	Toggle a grid on the window.
	\param gridSize: size of a square in the grid, in pixels
	\param alignBottomLeft: 1 to align the grid to bottom left corner, 0 if top left
	*/
	void gridToggle(int gridSize, bool alignBottomLeft);

	void multiplyEntity();

	////////////////////////////////////////////////////
	/*
		IMGUI INTERFACE
	*/
private:

	void mainMenu();
	void imGuiTextureMenu();
	void imGuiEditEntity();
	void imGuiListEntities();

	struct MainMenuTags
	{
		bool showTextureMenu = false;
		bool showListOfEntities = false;
		bool showEntityEditWindow = false;
	} _tagMenu;

	struct ListMenuTags
	{
		std::shared_ptr<Entity> selectedEntity;
	} _tagListMenu;
	////////////////////////////////////////////////////

	void sBBRender();
};
