#pragma once
#include "../../GameEngine.h"
#include "../../SceneABS.h"
#include <filesystem>
#include "../../Utils/messages.h"
#include "./LevelEditorTools/textureEditWindow.h"


typedef std::map<std::string, sf::Sprite> Sprites;

class PLevelEditor : public Scene
{
	//implement a circular queue for ctrl-z ctrl-y functions

	sf::Clock _deltaClock;
	sf::View _view;

public:

	PLevelEditor();
	PLevelEditor(GameEngine* gameEnginePointer);

	virtual void update() override;
	virtual void sDoAction(Action action) override;
	virtual void sRender() override;

	/*
	Initializes the level.
	Reads all textures in the Textures folder.
	*/
	void init();

	/*
	Saves the level configuration in the config file.
	*/
	void saveLevel(const std::string& path);

	/*
	Renders bounding boxes of all entities.
	*/
	void sBBRender();

	/*
	Reads the config file and loads all the entities.
	*/
	void readLevelCfgF(const std::string& path);

	/*
	Toggle a grid on the window.
	\param gridSize: size of a square in the grid, in pixels
	\param alignBottomLeft: 1 to align the grid to bottom left corner, 0 if top left
	*/
	void gridToggle(int gridSize, bool alignBottomLeft);

//////////////////////////////////////////////////////////////////////////
/*
IMGUI INTERFACE
This section contains all of ImGui menus and specific data they use.
*/
//////////////////////////////////////////////////////////////////////////

private:
	struct ImGuiVars
	{
		sf::Color background = sf::Color(137, 207, 240, 255);
		ImVec4 warningColor = { 1.0f, 1.0f, 0.0f, 1.0f };
		ImVec4 messageColor = { 0.0f, 1.0f, 1.0f, 1.0f };
	} _imGuiVars;

	//////////////////////////////////////////////////
	/*
	Main menu.
	Allows to turn on other windows.
	*/
	void mainMenu();

	struct MainMenuTags
	{
		bool showMakeSpriteWindow = false;
		bool showListOfEntities = false;
		bool showEntityEditWindow = false;
	} _tagMenu;

	//////////////////////////////////////////////////
	/*
	Save the selected sprite.
	\param sprite to be saved in the _sprites map.
	*/
	void saveSprite(sf::Sprite& sprite);

	void imGuiMakeSprite();
	Sprites _sprites;
	sf::Sprite _selectedSprite;
	sf::IntRect _spriteRect;
	char _spriteNameBuffer[50] = {""};
	std::string _selectedTexture;
	std::string _selectedSpriteInMap;

	struct TextureData
	{
		std::string texturePath;
		std::vector <std::string> textureNames;
	} _textureData;


	//////////////////////////////////////////////////
	/*
	List entities menu.
	Allows to list and select entities by tag.
	*/
	void imGuiListEntities();

	std::shared_ptr<Entity> selectedEntity = {nullptr};


	//////////////////////////////////////////////////
	/*	
	Edit entity menu.
	Allows to edit selected entities' components
	*/
	void imGuiEditEntity();

};
