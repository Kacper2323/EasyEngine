#include "platformerLevelEdit.h"
#include "../Utils/messages.h"
#include <fstream>
#include "PlayScene.h"

PLevelEditor::PLevelEditor() {};

PLevelEditor::PLevelEditor(GameEngine* gameEnginePointer)
	:Scene(gameEnginePointer), _textureWindow(TextureEditWindow(_game->window().getSize()))
{
	init();
};



void PLevelEditor::init()
{
	//register all necessary actions
	registerAction(sf::Mouse::Left + mouseButtonOFFSET, "LeftClick");
	registerAction(sf::Mouse::Right + mouseButtonOFFSET, "RightClick");
	registerAction(sf::Mouse::Middle + mouseButtonOFFSET, "MiddleClick");
	registerAction(mouseWheelOFFSET, "MouseWheel");

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
	_tagTexMenu.currentTexture = &_textureData.textureNames[0];

	readLevelCfgF("./cfgTemp.txt");
}



void PLevelEditor::update()
{
	_view.reset(sf::FloatRect(0, 0, 1280, 720));
	_game->window().setView(_view);
	_entities.Update();
	ImGui::SFML::Update(_game->window(), _deltaClock.restart());

	ImGui::ShowDemoWindow();
}

void PLevelEditor::readLevelCfgF(const std::string& path)
{
	std::fstream fin(path);
	std::string keyword;
	std::string component;

	while (fin >> keyword)
	{
		if (keyword == "ENTITY")
		{
			std::string entityTag;
			fin >> entityTag;
			auto entity = _entities.addEntity(entityTag);

			while (fin >> component)
			{
				if (component == "CBoundingBox")
				{
					Vec2 size;
					fin >> size.x >> size.y;
					entity->addComponent<CBoundingBox>(size);
				}
				else if (component == "CTransform")
				{
					Vec2 pos;
					Vec2 speed;
					float angle;
					fin >> pos.x >> pos.y >> speed.x >> speed.y >> angle;
					entity->addComponent<CTransform>(pos, speed, angle);
				}
				else if (component == "CSprite")
				{
					std::string textureName;
					sf::IntRect texRect;

					fin >> textureName >> texRect.left >> texRect.top >> texRect.width >> texRect.height;

					entity->addComponent<CSprite>(_game->getAssets().getTexture(textureName), texRect);
				}
				else if (component == "END")
				{
					break;
				}
				else
				{
					MSG::ERROR("Error in PlayScene cfg file: unknown keyword - ", component);
				}
			}
		}
	}
	fin.close();
}

void PLevelEditor::sDoAction(Action action)
{
	// user input actions within the texture window
	// if the texture window is on, no other actions in the game window are allowed to be performed
	if (_tagTexMenu.showTextureWindow)
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
		if (action.name() == "LeftClick" && action.type() == "START")
		{
			_textureWindow.startSelection(sf::Mouse::getPosition(_game->window()));
		}
		if (action.name() == "LeftClick" && action.type() == "END")
		{
			//the window has to be hidden for the sliders to work...
			MSG::TRACE("Sprite modyfied.");
			_tagTexMenu.showTextureWindow = false;
			sf::IntRect selection = _textureWindow.stopSelection();
			sf::Texture& usedTexture = _game->getAssets().getTexture(*_tagTexMenu.currentTexture);
			_selectedSprite = sf::Sprite(usedTexture, selection);
		}
	}
	else
	{

	}
}


void PLevelEditor::saveSprite(sf::Sprite& sprite)
{
	std::string spriteName = "sprite_" + std::to_string(_sprites.size());
	
	_sprites[spriteName] = sprite;
}



void PLevelEditor::sRender()
{
	_game->window().clear(_imGuiVars.background);
	gridToggle(64, false);

	//A VERY quick rendering scheme for testing, needs to change
	for (auto& e : _entities.getEntities())
	{
		if (e->getComponent<CTransform>().has)
		{
			e->getComponent<CSprite>().sprite.setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);
			e->getComponent<CSprite>().sprite.setRotation(e->getComponent<CTransform>().angle);
			_game->window().draw(e->getComponent<CSprite>().sprite);
		}
	}
	sBBRender();
	mainMenu();

	if (_tagTexMenu.showTextureWindow)
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

void PLevelEditor::saveLevel(const std::string& path)
{
	std::ofstream cfgFile(path, std::ofstream::out | std::ofstream::trunc);

	for (auto e : _entities.getEntities())
	{
		if (e->tag() == "boundry")
		{
			continue;
		}

		cfgFile << "ENTITY " << e->tag() << std::endl;
		if (e->getComponent<CTransform>().has)
		{
			auto& cT = e->getComponent<CTransform>();
			cfgFile << "CTransform " << cT.pos.x << " " << cT.pos.y << " ";
			cfgFile << cT.velocity.x << " " << cT.velocity.y << " " << cT.angle << std::endl;
		}

		if (e->getComponent<CBoundingBox>().has)
		{
			auto& cBB = e->getComponent<CBoundingBox>();
			cfgFile << "CBoundingBox " << cBB.size.x << " " << cBB.size.y << std::endl;
		}

		if (e->getComponent<CSprite>().has)
		{
			auto& cS = e->getComponent<CSprite>();
			std::string textureName;
			cfgFile << "CSprite ";
			for (auto& [k, v] : _game->getAssets().getTextures())
			{
				if (cS.sprite.getTexture() == &v)
				{
					cfgFile << k << " ";
				}
			}
			sf::IntRect texRect = cS.sprite.getTextureRect();
			cfgFile << texRect.left << " " << texRect.top << " ";
			cfgFile << texRect.width << " " << texRect.height << std::endl;
		}

		cfgFile << "END" << std::endl;
	}
	cfgFile.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
IMGUI MENUS
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////


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

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if (ImGui::Button("Save level"))
	{
		saveLevel("./cfgTemp.txt");
	}
	ImGui::SameLine();
	if (ImGui::Button("Play"))
	{
		_game->changeScene("PlayScene", std::make_shared<PlayScene>(_game), 0);
	}

	ImGui::End();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

void PLevelEditor::imGuiListEntities()
{
	ImGui::Begin("Entity menu");

	ImGui::SameLine(ImGui::GetWindowWidth() - 50);	//close button (50 is the position from the right, picked based on button width)
	if (ImGui::Button("Close##1"))
	{
		_tagMenu.showListOfEntities = false;
	}

	std::vector<std::string> tagTypes = _entities.getTags();
	
	if (ImGui::BeginTabBar("Entity_tag_types", ImGuiTabBarFlags_None))
	{

		for (auto& name : tagTypes)	//for every tag in _entities create a bar in bartabs
		{
			if (ImGui::BeginTabItem(name.c_str()))
			{
				//in every bar tab create a selectable list of entities belonging to that tag
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
						{
							_tagListMenu.selectedEntity = e;
							for (auto obj : _entities.getEntities())
							{
								obj->getComponent<CBoundingBox>().physical = 1;
							}
							e->getComponent<CBoundingBox>().physical = 0;
						}

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Find the size of the sprite so that one of the dimentions will be [maxSize]	and the other will be scaled.
It keeps the image width to height ratio. spriteSize is the textureRect of the sprite.
Made to be used with ImGui Image() which requires size as input.
*/
static sf::Vector2f spriteResize(const sf::IntRect& spriteSize, int maxSize)
{
	sf::Vector2f imageSize = { 0.0f, 0.0f };
	float sizeRatio = (float)spriteSize.width / spriteSize.height;

	if (sizeRatio >= 1)
	{
		imageSize.x = maxSize;
		imageSize.y = maxSize / sizeRatio;
	}
	else
	{
		imageSize.y = maxSize;
		imageSize.x = maxSize * sizeRatio;
	}

	return imageSize;
}

void PLevelEditor::sBBRender()
{
	//Must be called between window.clear() and window.display() in a routine

	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{
		if (e->getComponent<CBoundingBox>().has)
		{
			Vec2 bbSize = e->getComponent<CBoundingBox>().size;
			Vec2 bbPos = e->getComponent<CTransform>().pos;

			sf::RectangleShape bb(sf::Vector2f(bbSize.x, bbSize.y));
			bb.setPosition(bbPos.x, bbPos.y);
			bb.setOrigin(bbSize.x / 2, bbSize.y / 2);
			bb.setFillColor(sf::Color::Transparent);
			if (e->getComponent<CBoundingBox>().physical)
				bb.setOutlineColor(sf::Color::Red);
			else
				bb.setOutlineColor(sf::Color::Cyan);
			bb.setOutlineThickness(1);
			_game->window().draw(bb);
		}
	}
}

void PLevelEditor::imGuiEditEntity()
{
	ImGui::Begin("Entity Creator");
	
	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Close##1"))
	{
		_tagMenu.showEntityEditWindow = false;
	}

	if (!_tagListMenu.selectedEntity)
	{
		ImGui::TextColored(_imGuiVars.warningColor, "Please select an entity.");
		ImGui::End();
		return;
	}

	std::shared_ptr<Entity> entity = _tagListMenu.selectedEntity;

	sf::IntRect spriteSize = entity->getComponent<CSprite>().sprite.getTextureRect();

	sf::Vector2f imageSize = spriteResize(spriteSize, 100);

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
			else //if entity has CTransform
			{
				auto& comp = entity->getComponent<CTransform>();

				ImGui::PushItemWidth(200);
				ImGui::DragFloat("Position on X axis", &comp.pos.x);
				ImGui::PushItemWidth(200);
				ImGui::DragFloat("Position on Y axis", &comp.pos.y);
				ImGui::PushItemWidth(100);
				ImGui::SliderFloat("Rotation", &comp.angle, 0, 360);

			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Physics"))
		{	
			if (!entity->getComponent<CBoundingBox>().has)
			{
				ImGui::TextColored(_imGuiVars.warningColor, "No Bounding Box component!");
				if (ImGui::Button("Add##1"))
				{
					entity->addComponent<CBoundingBox>();
				}
			}
			else //if entity has a bounding box
			{
				auto& comp = entity->getComponent<CBoundingBox>();

				ImGui::PushItemWidth(100);
				ImGui::DragFloat("BB width", &comp.size.x);
				ImGui::SameLine(); ImGui::PushItemWidth(100);
				ImGui::DragFloat("BB height", &comp.size.y);
				ImGui::Checkbox("Physical?", &comp.physical);

			}
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void PLevelEditor::imGuiTextureMenu()
{
	/////	BEGIN MENU		/////
	ImGui::Begin("Sprite Creator");

	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Close##1"))
	{
		_tagMenu.showTextureMenu = false;
		_tagTexMenu.showTextureWindow = false;
	}

	ImGui::Text("Texture loader");
	if (!_textureData.textureNames.empty())
	{
		/////	COMBO MENU TO PICK A TEXTURE	/////
		if (ImGui::BeginCombo("##texture_loader", _tagTexMenu.currentTexture->c_str()))
		{
			for (std::string& name : _textureData.textureNames)
			{
				bool is_selected = (&name == _tagTexMenu.currentTexture);
				if (ImGui::Selectable(name.c_str(), is_selected))
				{
					_tagTexMenu.currentTexture = &name;
					_tagTexMenu.showTextureWindow = false;
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
		if (_game->getAssets().getTextures().find(*_tagTexMenu.currentTexture) == _game->getAssets().getTextures().end())
		{
			if (ImGui::Button("load texture")) {
				_game->getAssets().addTexture(*_tagTexMenu.currentTexture, _textureData.texturePath + *_tagTexMenu.currentTexture);
			}
		}
		else
		{
			ImGui::Text("Texture ready");
		}

		/////	BUTTON TO SHOW/HIDE A LOADED TEXTURE		/////
		if (!_tagTexMenu.showTextureWindow)
		{
			if (ImGui::Button("show texture"))
			{
				if (_tagTexMenu.currentTexture == nullptr)
				{
					MSG::ERROR("currentTexture in _imGuiVars is a nullptr");
					_tagTexMenu.showTextureWindow = false;
				}
				else if (_game->getAssets().getTextures().find(*_tagTexMenu.currentTexture) == _game->getAssets().getTextures().end())
				{
					MSG::ERROR(*_tagTexMenu.currentTexture, "in currentTexture is not loaded into editors assets");
					_tagTexMenu.showTextureWindow = false;
				}
				else
				{
					_textureWindow.useTexture(_game->getAssets().getTexture(*_tagTexMenu.currentTexture));
					_tagTexMenu.showTextureWindow = true;
					_textureWindow.resetView();
				}
			}
		}
		else
		{
			if (ImGui::Button("hide texture"))
			{
				_tagTexMenu.showTextureWindow = false;
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

		ImGui::TextColored(sf::Color::Yellow, "Window has to be hidden to modify these:");
		ImGui::Text("sprite rect position: ");
		ImGui::PushItemWidth(100);
		ImGui::DragInt("X", &spriteSize.left);
		ImGui::SameLine(); ImGui::PushItemWidth(100);
		ImGui::DragInt("Y", &spriteSize.top);

		ImGui::Text("sprite rect size: ");
		ImGui::PushItemWidth(100);
		ImGui::DragInt("##spriteWidth", &spriteSize.width);
		ImGui::SameLine(); ImGui::PushItemWidth(100);
		ImGui::DragInt("##spriteHeight", &spriteSize.height);
		_selectedSprite.setTextureRect(spriteSize);

		sf::Vector2f imageSize = spriteResize(spriteSize, 250);

		ImGui::Image(_selectedSprite, imageSize, sf::Color::White, sf::Color::White);
		
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

	if (ImGui::BeginCombo("##sprite_picker", _tagTexMenu.currentSprite.c_str()))
	{
		std::map<std::string, sf::Sprite>::iterator it = _sprites.begin();

		while (it != _sprites.end())
		{
			bool is_selected = (it->first == _tagTexMenu.currentSprite);
			if (ImGui::Selectable(it->first.c_str(), is_selected))
			{
				_tagTexMenu.currentSprite = it->first;
				_selectedSprite = it->second;
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

	if (!_sprites.empty() && _tagTexMenu.currentSprite != "")
	{
		if (ImGui::Button("create entity with sprite"))
		{

			_tagListMenu.selectedEntity = _entities.addEntity(std::string(entityTag));
			_tagListMenu.selectedEntity->addComponent<CSprite>(_sprites[_tagTexMenu.currentSprite]);
		}
	}
	else
	{
		ImGui::TextColored(_imGuiVars.messageColor, "Pick a sprite to create an entity.");
	}
	
	ImGui::End();
}