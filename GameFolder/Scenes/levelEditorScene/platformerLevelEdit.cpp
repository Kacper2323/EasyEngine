#include "platformerLevelEdit.h"
#include "../../Utils/messages.h"
#include "../../Utils/mathStuff.h"
#include <fstream>
#include "../PlayScene/PlayScene.h"

PLevelEditor::PLevelEditor() {};

PLevelEditor::PLevelEditor(GameEngine* gameEnginePointer)
	:Scene(gameEnginePointer)
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

	readLevelCfgF("./cfgTemp.txt");

	//initialize _selectedTexture to first element in textureNames for the sprite picker
	_selectedTexture = _textureData.textureNames[0];
}



void PLevelEditor::update()
{
	_view.reset(sf::FloatRect(0, 0, 1280, 720));
	_game->window().setView(_view);
	_entities.Update();
	ImGui::SFML::Update(_game->window(), _deltaClock.restart());

	ImGui::ShowDemoWindow();
}


void PLevelEditor::sDoAction(Action action)
{
	// user input actions within the texture window
	// if the texture window is on, no other actions in the game window are allowed to be performed

	if (action.name() == "LeftClick" && action.type() == "START")
	{
		for (auto e : _entities.getEntities())
		{
			auto& eBB = e->getComponent<CBoundingBox>();
			auto& cT = e->getComponent<CTransform>();

			//if mouse pos is outside of object e on x axis
			if (!(action.mouseX > cT.pos.x - eBB.halfSize.x && action.mouseX < cT.pos.x + eBB.halfSize.x))
				continue;
			
			//if mouse pos is outside of object e on y axis
			if (!(action.mouseY > cT.pos.y - eBB.halfSize.y && action.mouseY < cT.pos.y + eBB.halfSize.y))
				continue;

			//if mouse is within the object
			selectedEntity = e;

			for (auto obj : _entities.getEntities())
			{
				obj->getComponent<CBoundingBox>().physical = 1;
			}
			e->getComponent<CBoundingBox>().physical = 0;
		}
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

	if (_tagMenu.showMakeSpriteWindow)
	{
		imGuiMakeSprite();
	}

	if (_tagMenu.showEntityEditWindow)
	{
		imGuiEditEntity();
	}

	if (_tagMenu.showListOfEntities)
	{
		imGuiListEntities();
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

	if (ImGui::Button("Create sprites"))
	{
		_tagMenu.showMakeSpriteWindow = true;
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
	if (ImGui::Button("Close##0"))
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
						const bool isSelected = (selectedEntity == e);

						std::string label("id: ");
						label += std::to_string(e->id());
						label += "  is alive: ";
						label += e->is_alive() ? "true" : "false";

						if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
						{
							selectedEntity = e;
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


void PLevelEditor::imGuiEditEntity()
{
	ImGui::Begin("Entity Creator");
	
	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Close##1"))
	{
		_tagMenu.showEntityEditWindow = false;
	}

	if (!selectedEntity)
	{
		ImGui::TextColored(_imGuiVars.warningColor, "Please select an entity.");
		ImGui::End();
		return;
	}

	std::shared_ptr<Entity> entity = selectedEntity;

	sf::IntRect spriteSize = entity->getComponent<CSprite>().sprite.getTextureRect();

	sf::Vector2f imageSize = spriteResize(spriteSize, 100);

	ImGui::Image(selectedEntity->getComponent<CSprite>().sprite,
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
		if (ImGui::BeginTabItem("Sprite"))
		{

			if(!_sprites.empty())
			{ 
				
				ImGui::TextColored(_imGuiVars.messageColor, "Pick a sprite:");
				if (ImGui::BeginCombo("##List2", _selectedSpriteInMap.c_str()))
				{

					for (auto e : _sprites)
					{
						const bool isSelected = (_selectedSpriteInMap == e.first);

						if (ImGui::Selectable(e.first.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
						{
							_selectedSpriteInMap = e.first;
							_selectedSprite = e.second;
							_spriteRect = _selectedSprite.getTextureRect();
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::Image(_selectedSprite, spriteResize(_selectedSprite.getTextureRect(), 50));

				if (ImGui::Button("Select"))
				{
					selectedEntity->addComponent<CSprite>(_selectedSprite);
				}

			}
			else
			{
				ImGui::Text("No sprites found");
			}

			if (ImGui::Button("Create a new sprite"))
			{
				_tagMenu.showMakeSpriteWindow = true;
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

void PLevelEditor::imGuiMakeSprite()
{
	static bool resetFlag = 1; //reset the size of the displayed texture rect
							   //to the entire texture

	ImGui::Begin("Make Sprite Tool");

	ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if (ImGui::Button("Close##2"))
	{
		_tagMenu.showMakeSpriteWindow = false;
	}

	if (_textureData.textureNames.empty())
	{
		ImGui::TextColored(_imGuiVars.warningColor, "No textures found in path:");
		ImGui::TextColored(_imGuiVars.warningColor, _textureData.texturePath.c_str());
		ImGui::End();
		return;
	}

	ImGui::Text("Pick a texture or load a new one.");
	if (ImGui::BeginCombo("##List2", _selectedTexture.c_str()))
	{

		for (auto& e : _textureData.textureNames)
		{
			const bool isSelected = (_selectedTexture == e);

			if (ImGui::Selectable(e.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
			{
				_selectedTexture = e;
				resetFlag = 1;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if (_game->getAssets().getTextures().find(_selectedTexture) != _game->getAssets().getTextures().end())
	{
		ImGui::Text("Loaded");

		sf::Vector2u texSize = _game->getAssets().getTexture(_selectedTexture).getSize();

		if (resetFlag) //this resets the displayed texture rect to the whole texture
		{
			_selectedSprite = sf::Sprite(_game->getAssets().getTexture(_selectedTexture));
			_spriteRect = sf::IntRect(0, 0, texSize.x, texSize.y);
			resetFlag = 0;
		}

		sf::Vector2f imageSize = spriteResize(_spriteRect, 400);

		// Show selected texture in a window
		// selected texture is initialized in the init method
		ImGui::Image(_selectedSprite, imageSize, sf::Color::White, sf::Color::White);

		//create a menu that allows to change the rect of the sprite
		ImGui::SetCursorPosY(500);

		ImGui::PushItemWidth(150);
		ImGui::SliderInt("##X pos", &_spriteRect.left, 0, texSize.x-1);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
		{ _spriteRect.left = MATH::clamp(0, _spriteRect.left-1, texSize.x-1); }
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
		{ _spriteRect.left = MATH::clamp(0, _spriteRect.left + 1, texSize.x - 1); }
		ImGui::SameLine();
		ImGui::Text("Left");
		ImGui::SameLine(0.0f, 40.0f);

		ImGui::PushItemWidth(150);
		ImGui::SliderInt("##Y pos", &_spriteRect.top, 0, texSize.y-1);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##top", ImGuiDir_Left)) 
		{ _spriteRect.top = MATH::clamp(0, _spriteRect.top - 1, texSize.y - 1); }
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##down", ImGuiDir_Right)) 
		{ _spriteRect.top = MATH::clamp(0, _spriteRect.top + 1, texSize.y - 1); }
		ImGui::SameLine();
		ImGui::Text("Top");
		
		ImGui::PushItemWidth(150);
		ImGui::SliderInt("##width", &_spriteRect.width, 0, texSize.x-1);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##width-", ImGuiDir_Left)) 
		{ _spriteRect.width = MATH::clamp(0, _spriteRect.width - 1, texSize.x - 1); }
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##width+", ImGuiDir_Right)) 
		{ _spriteRect.width = MATH::clamp(0, _spriteRect.width + 1, texSize.x - 1); }
		ImGui::SameLine();
		ImGui::Text("Width");
		ImGui::SameLine(0.0f, 33.0f);

		ImGui::PushItemWidth(150);
		ImGui::SliderInt("##height", &_spriteRect.height, 0, texSize.y-1);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##height-", ImGuiDir_Left)) 
		{ _spriteRect.height = MATH::clamp(0, _spriteRect.height - 1, texSize.y - 1); }
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##height+", ImGuiDir_Right)) 
		{ _spriteRect.height = MATH::clamp(0, _spriteRect.height + 1, texSize.y - 1); }
		ImGui::SameLine();
		ImGui::Text("Height");
		
		_selectedSprite.setTextureRect(_spriteRect);

		ImGui::Dummy(ImVec2(0, 20));
		ImGui::Text("Sprite name:");
		ImGui::InputText("##spriteName", _spriteNameBuffer, IM_ARRAYSIZE(_spriteNameBuffer));

		std::string spriteName(_spriteNameBuffer);
		if (ImGui::Button("save sprite"))
		{
			if (spriteName != "")
				_sprites[spriteName] = _selectedSprite;
			else
				MSG::ERROR("Input a sprite name before saving");
		}
		if (_sprites.find(spriteName) != _sprites.end())
		{
			ImGui::SameLine();
			ImGui::TextColored(sf::Color::Green, "Saved!");
		}
	}
	else
	{
		if (ImGui::Button("Load"))
		{
			_game->getAssets().addTexture(_selectedTexture, _textureData.texturePath + _selectedTexture);
		}
		ImGui::TextColored(_imGuiVars.messageColor, "Texture is not loaded.");
	}

	
	ImGui::End();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

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