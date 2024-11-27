#pragma once

#include <raylib.h>

#include "../GameEngine.h"
#include "../SceneABS.h"
#include <filesystem>
#include "../Utils/messages.h"



typedef std::map<std::string, Sprite> Sprites;

class PLevelEditor : public Scene
{
	//implement a circular queue for ctrl-z ctrl-y functions

	Camera2D _camera = { 0 };
public:

	PLevelEditor();
	PLevelEditor(GameEngine* gameEnginePointer);

	virtual void update() override;
	virtual void sDoAction(Action action) override;
	virtual void sRender() override;

private:
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

	//////	ImGui char buffers	//////
	char _newEntityTag[20] = {""}; 		//tag buffer for new entity name field
	char _newCfgNameBuffer[25] = {""};	//buffer for the new cfg file name field
	char _spriteNameBuffer[50] = {""};	//buffer for the sprite name field

	//////	ImGui selection variables	//////
	std::string _currentCfgFile = {""}; //config file selected as the current one in the menu
	std::string _selectedTexture;		//currently selected texture (texture loader/picker)
	std::string _selectedSpriteInMap;	//currently selected sprite (name) (in sprite map<str, Sprite> for entity modification)
	Sprite _selectedSprite;				//currently selected sprite in editor
	Rectangle _spriteRect;				//rect of currently selected sprite (within texture)
	std::shared_ptr<Entity> selectedEntity = {nullptr};	//currently selected entity

	//////	containers	//////
	Sprites _sprites;	//map of all created sprites with names
	std::vector<std::string> configFiles; //vector containing all config files

	/////	asset paths	//////
	std::string cfgDirPath = {"../Assets/cfgFiles/"}; //path to cfgFiles directory
	struct TextureData
	{
		std::string texturePath;
		std::vector <std::string> textureNames;
	} _textureData;	/// path to the texture dir and texture names within

	struct ImGuiVars
	{
		Color background = { 137, 207, 240, 255 };
		ImVec4 warningColor = { 1.0f, 1.0f, 0.0f, 1.0f };
		ImVec4 messageColor = { 0.0f, 1.0f, 1.0f, 1.0f };
	} _imGuiVars; /// misc variables for ImGui

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
		bool panningFlag = false;
	} _tagMenu;

	//////////////////////////////////////////////////
	/*
	Save the selected sprite.
	\param sprite to be saved in the _sprites map.
	*/
	void saveSprite(Sprite& sprite);

	void imGuiMakeSprite();

	//////////////////////////////////////////////////
	/*
	List entities menu.
	Allows to list and select entities by tag.
	*/
	void imGuiListEntities();

	//////////////////////////////////////////////////
	/*	
	Edit entity menu.
	Allows to edit selected entities' components
	*/
	void imGuiEditEntity();

};
