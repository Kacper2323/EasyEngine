#include "platformerLevelEdit.h"
#include "../Utils/messages.h"

PLevelEditor::PLevelEditor() {};

PLevelEditor::PLevelEditor(GameEngine* gameEnginePointer)
	:Scene(gameEnginePointer), _textureWindow(TextureEditWindow(_game->window().getSize()))
{
	registerAction(sf::Mouse::Left + mouseButtonOFFSET, "LeftClick");
	registerAction(sf::Mouse::Right + mouseButtonOFFSET, "RightClick");
	registerAction(sf::Mouse::Middle + mouseButtonOFFSET, "MiddleClick");
	registerAction(mouseWheelOFFSET, "MouseWheel");
	init();
};



void PLevelEditor::init()
{
	//initialize a list of available textures in _textureData
	_textureData.texturePath = "./GameFolder/Assets/Textures/";
	std::string fileName;
	for(const auto& entry : std::filesystem::directory_iterator(_textureData.texturePath))
	{
		fileName = entry.path().filename().string();

		std::cout << fileName << std::endl;
		_textureData.textureNames.push_back(fileName);
	}
	if (_textureData.textureNames.empty()) { return; };
	_imGuiVars.currentTexture = &_textureData.textureNames[0];
}



void PLevelEditor::update()
{
	_entities.Update();
	ImGui::SFML::Update(_game->window(), _deltaClock.restart());

	ImGui::ShowDemoWindow();
}



void PLevelEditor::sDoAction(Action action)
{
	// user input actions within the texture window
	// if the texture window is on, no other actions in the game window are allowed to be performed
	if (_imGuiVars.showTextureWindow)
	{
		if (action.name() == "MiddleClick" && action.type() == "START")
		{
			_textureWindow.setPanning(sf::Vector2i(action.mouseX, action.mouseY));
		}
		if (action.name() == "MiddleClick" && action.type() == "END")
		{
			_textureWindow.stopPanning();
		}
		if (action.name() == "MouseWheel")
		{
			_textureWindow.zoom(action.mouseWheelDelta, action.mouseX, action.mouseY);
		}
		if (action.name() == "RightClick" && action.type() == "START")
		{
			_textureWindow.startSelection(sf::Mouse::getPosition(_game->window()));
		}
		if (action.name() == "RightClick" && action.type() == "END")
		{
			sf::IntRect selection = _textureWindow.stopSelection();
			sf::Texture& usedTexture = _editorAssets.getTexture(*_imGuiVars.currentTexture);
			_selectedSprite = sf::Sprite(usedTexture, selection);
		}
	}
	else
	{

	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Section for working with sprites.
Related objects and functions:
	_sprites
	_selectedSprite
	saveSprite()
*/

void PLevelEditor::saveSprite(sf::Sprite& sprite)
{
	std::string spriteName = "sprite_" + std::to_string(_sprites.size());

	_sprites[spriteName] = sprite;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////


void PLevelEditor::sRender()
{
	_game->window().clear(_imGuiVars.background);
	gridToggle(64, false);

	//A VERY quick rendering scheme for testing, needs to change
	for (auto& e : _entities.getEntities())
	{
		if (e->getComponent<CTransform>().has)
		{
			_game->window().draw(e->getComponent<CSprite>().sprite);
			e->getComponent<CSprite>().sprite.setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);
		}
	}

	mainMenu();

	if (_imGuiVars.showTextureWindow)
	{
		_textureWindow.sWindowRender(_game->window());
	}
	if (_tagMenu.showEntityEditWindow)
	{
		imGuiEditEntity();
	}
	if (_tagMenu.showListOfEntities)
	{
		imGuiListEntities();
	}
	if (_tagMenu.showTextureMenu)
	{
		imGuiTextureMenu();
	}

	ImGui::SFML::Render(_game->window());
	_game->window().display();
}



/*
	Draws a square grid with [gridSize] pixels spacing.
	aligned to either bottom-left or top-left
*/
void PLevelEditor::gridToggle(int gridSize, bool alignBottomLeft)
{
	sf::Vector2u windowSize = _game->window().getSize();

	sf::Vertex line[2];

	for (int i = 1; i < windowSize.x; i += gridSize)
	{
		line[0].position = sf::Vector2f(i, 0);
		line[0].color = sf::Color::White;
		line[1].position = sf::Vector2f(i, windowSize.y);
		line[1].color = sf::Color::White;
		_game->window().draw(line, 2, sf::Lines);
	}

	if (alignBottomLeft)
	{
		for (int i = windowSize.y; i > 0; i -= gridSize)
		{
			line[0].position = sf::Vector2f(0, i);
			line[0].color = sf::Color::White;
			line[1].position = sf::Vector2f(windowSize.x, i);
			line[1].color = sf::Color::White;
			_game->window().draw(line, 2, sf::Lines);
		}
	}
	else
	{
		for (int i = 0; i < windowSize.y; i += gridSize)
		{
			line[0].position = sf::Vector2f(0, i);
			line[0].color = sf::Color::White;
			line[1].position = sf::Vector2f(windowSize.x, i);
			line[1].color = sf::Color::White;
			_game->window().draw(line, 2, sf::Lines);
		}
	}
}



void PLevelEditor::mainMenu()
{
	ImGui::Begin("Main Menu");

	ImGui::Text("This is the main menu.");
	ImGui::Text("Select what you want to do.");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if (ImGui::Button("Load textures"))
	{
		_tagMenu.showTextureMenu = true;
	}


	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if (ImGui::Button("List existing entities"))
	{
		_tagMenu.showListOfEntities = true;
	}


	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if (ImGui::Button("Edit entity"))
	{
		_tagMenu.showEntityEditWindow = true;
	}

	ImGui::End();
}

void PLevelEditor::imGuiListEntities()
{
	ImGui::Begin("Entity menu");

	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Close##1"))
	{
		_tagMenu.showListOfEntities = false;
	}

	std::vector<std::string> tagTypes = _entities.getTags();
	
	if (ImGui::BeginTabBar("Entity_tag_types", ImGuiTabBarFlags_None))
	{

		for (auto& name : tagTypes)
		{
			if (ImGui::BeginTabItem(name.c_str()))
			{

				if (ImGui::BeginListBox("##List", ImVec2(-FLT_MIN, 15 * ImGui::GetTextLineHeightWithSpacing())))
				{
					EntityVec& entities = _entities.getEntities(name);

					for (auto& e : entities)
					{
						const bool isSelected = (_tagListMenu.selectedEntity == e);

						std::string label("id: ");
						label += std::to_string(e->id());
						label += "  is alive: ";
						label += e->is_alive() ? "true" : "false";

						if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
							_tagListMenu.selectedEntity = e;

						if(isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndListBox();
				}

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}


void PLevelEditor::imGuiEditEntity()
{
	ImGui::Begin("Entity Creator");

	std::shared_ptr<Entity> entity = _tagListMenu.selectedEntity;

	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Close##1"))
	{
		_tagMenu.showEntityEditWindow = false;
	}

	sf::IntRect spriteSize = _tagListMenu.selectedEntity->getComponent<CSprite>().sprite.getTextureRect();

	sf::Vector2f imageSize = { 0.0f, 0.0f };
	int maxImageSize = 500;

	float sizeRatio = (float)spriteSize.width / spriteSize.height;

	if (sizeRatio >= 1)
	{
		imageSize.x = maxImageSize;
		imageSize.y = maxImageSize / sizeRatio;
	}
	else
	{
		imageSize.y = maxImageSize;
		imageSize.x = maxImageSize * sizeRatio;
	}

	ImGui::Image(_tagListMenu.selectedEntity->getComponent<CSprite>().sprite,
				 imageSize, sf::Color::White, sf::Color::White);

	ImGui::SameLine();

	ImGui::BeginGroup();
	
	if (ImGui::BeginTabBar("Components", ImGuiTabBarFlags_None))
	{

			if (ImGui::BeginTabItem("Transform"))
			{

				if (!entity->getComponent<CTransform>().has)
				{
					ImGui::TextColored(_imGuiVars.warningColor, "No transform component!");
					if (ImGui::Button("Add##1"))
					{
						entity->addComponent<CTransform>();
					}
				}
				else
				{
					auto& comp = entity->getComponent<CTransform>();

					ImGui::SliderFloat("Position X axis", &comp.pos.x, 0, 500);

				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Physics"))
			{
				
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Input"))
			{
				
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Misc"))
			{

				ImGui::EndTabItem();
			}

		ImGui::EndTabBar();
	}

	ImGui::EndGroup();
	ImGui::End();
}

void PLevelEditor::imGuiTextureMenu()
{
	/////	BEGIN MENU		/////
	ImGui::Begin("Sprite Creator");

	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Close##1"))
	{
		_tagMenu.showTextureMenu = false;
		_imGuiVars.showTextureWindow = false;
	}

	ImGui::Text("Texture loader");
	if (!_textureData.textureNames.empty())
	{
		/////	COMBO MENU TO PICK A TEXTURE	/////
		if (ImGui::BeginCombo("##texture_loader", _imGuiVars.currentTexture->c_str()))
		{
			for (std::string& name : _textureData.textureNames)
			{
				bool is_selected = (&name == _imGuiVars.currentTexture);
				if (ImGui::Selectable(name.c_str(), is_selected))
				{
					_imGuiVars.currentTexture = &name;
					_imGuiVars.showTextureWindow = false;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();

		/////	BUTTON TO LOAD A TEXTURE	/////
		if (_editorAssets.getTextures().find(*_imGuiVars.currentTexture) == _editorAssets.getTextures().end())
		{
			if (ImGui::Button("load texture")) {
				_editorAssets.addTexture(*_imGuiVars.currentTexture, _textureData.texturePath + *_imGuiVars.currentTexture);
			}
		}
		else
		{
			ImGui::Text("Texture ready");
		}

		/////	BUTTON TO SHOW/HIDE A LOADED TEXTURE		/////
		if (!_imGuiVars.showTextureWindow)
		{
			if (ImGui::Button("show texture"))
			{
				if (_imGuiVars.currentTexture == nullptr)
				{
					MSG::ERROR("currentTexture in _imGuiVars is a nullptr");
					_imGuiVars.showTextureWindow = false;
				}
				else if (_editorAssets.getTextures().find(*_imGuiVars.currentTexture) == _editorAssets.getTextures().end())
				{
					MSG::ERROR(*_imGuiVars.currentTexture, "in currentTexture is not loaded into editors assets");
					_imGuiVars.showTextureWindow = false;
				}
				else
				{
					_textureWindow.useTexture(_editorAssets.getTexture(*_imGuiVars.currentTexture));
					_imGuiVars.showTextureWindow = true;
				}
			}
		}
		else
		{
			if (ImGui::Button("hide texture"))
			{
				_imGuiVars.showTextureWindow = false;
			}
		}
	}
	else	//if the textureNames vector is empty
	{
		ImGui::TextColored(_imGuiVars.warningColor, "No textures found in path: ");
		ImGui::Text(_textureData.texturePath.c_str());
	}


	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	/////	DISPLAY THE SELECTED SPRITE		/////
	ImGui::Text("Selected sprite: ");

	sf::IntRect spriteSize = _selectedSprite.getTextureRect();
	if (spriteSize.width != 0)
	{
		sf::Vector2f imageSize = { 0.0f, 0.0f };
		int maxImageSize = 150;

		float sizeRatio = (float)spriteSize.width / spriteSize.height;

		if (sizeRatio >= 1)
		{
			imageSize.x = maxImageSize;
			imageSize.y = maxImageSize / sizeRatio;
		}
		else
		{
			imageSize.y = maxImageSize;
			imageSize.x = maxImageSize * sizeRatio;
		}

		ImGui::Image(_selectedSprite, imageSize, sf::Color::White, sf::Color::White);
		/*ImGui::SameLine();
		ImVec2 sPos = ImGui::GetCursorScreenPos();
		
		MSG::TRACE("sPos: ", sPos.x, " : ", imageSize.x);
		sPos.x = sPos.x - imageSize.x - 10;
		ImGui::SetCursorScreenPos(sPos);
		MSG::TRACE("sPosXNew: ", sPos.x);
		ImGui::Image(_selectedSprite, imageSize, sf::Color::White, sf::Color::Red);*/
		
		if (ImGui::Button("Save sprite"))
		{
			saveSprite(_selectedSprite);
		}
	}
	else
	{
		ImGui::TextColored(_imGuiVars.messageColor, "No sprite selected.");
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	/////	COMBO TO PICK A SPRITE	/////
	ImGui::Text("Saved sprites:");

	if (ImGui::BeginCombo("##sprite_picker", _imGuiVars.currentSprite.c_str()))
	{
		std::map<std::string, sf::Sprite>::iterator it = _sprites.begin();

		while (it != _sprites.end())
		{
			bool is_selected = (it->first == _imGuiVars.currentSprite);
			if (ImGui::Selectable(it->first.c_str(), is_selected))
			{
				_imGuiVars.currentSprite = it->first;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}

			it++;
		}
		ImGui::EndCombo();
	}

	ImGui::Text("Preview: ");

	static char entityTag[50];
	ImGui::Text("Input a tagname for the entity: ");
	ImGui::InputText("##tagneme_input", entityTag, 50);

	if (!_sprites.empty() && _imGuiVars.currentSprite != "")
	{
		if (ImGui::Button("create entity with sprite"))
		{

			_tagListMenu.selectedEntity = _entities.addEntity(std::string(entityTag));
			_tagListMenu.selectedEntity->addComponent<CSprite>(_sprites[_imGuiVars.currentSprite]);
		}
	}
	else
	{
		ImGui::TextColored(_imGuiVars.messageColor, "Pick a sprite to create an entity.");
	}
	
	/////	END		/////
	ImGui::End();
}