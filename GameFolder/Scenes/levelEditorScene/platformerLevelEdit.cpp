#include "platformerLevelEdit.h"
#include "../../Utils/messages.h"
#include "../../Utils/mathStuff.h"
#include "../../Utils/IOData.h"
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
	registerAction(MOUSE_BUTTON_LEFT + mouseButtonOFFSET, "LeftClick");
	registerAction(MOUSE_BUTTON_RIGHT + mouseButtonOFFSET, "RightClick");
	registerAction(MOUSE_BUTTON_MIDDLE + mouseButtonOFFSET, "MiddleClick");
	registerAction(mouseWheelOFFSET, "MouseWheel");

	//init the view
	_camera.offset.x = GetScreenWidth() / 2;
	_camera.offset.y = GetScreenHeight() / 2;
	_camera.zoom = 1;
	_camera.target.x = GetScreenWidth() / 2.0f;
	_camera.target.y = GetScreenHeight() / 2.0f;

	//initialize a list of available textures in _textureData
	_textureData.texturePath = "./GameFolder/Assets/Textures/";

	_textureData.textureNames = IO::listFiles(_textureData.texturePath);
	if (_textureData.textureNames.empty()) { return; };

	readLevelCfgF("./cfgTemp.cfg");
	
	//initialize _selectedTexture to first element in textureNames for the sprite picker
	_selectedTexture = _textureData.textureNames[0];
}



void PLevelEditor::update()
{
	//panning functionality
	if (_tagMenu.panningFlag)
	{

		Vector2 mouseDelta = GetMouseDelta();

		_camera.offset.x += mouseDelta.x;
		_camera.offset.y += mouseDelta.y;
	}

	_entities.Update();
	
}


void PLevelEditor::sDoAction(Action action)
{
	if (action.name() == "LeftClick" && action.type() == "START" && !ImGui::GetIO().WantCaptureMouse)
	{
		for (auto e : _entities.getEntities())
		{
			auto& eBB = e->getComponent<CBoundingBox>();
			auto& cT = e->getComponent<CTransform>();

			//get the position of the mouse on the screen
			Vector2 mousePos(action.mouseX, action.mouseY);

			//map the position in the world coordinates (the screen can show zoomed in or moved view)
			Vector2 pixelPos = { GetScreenToWorld2D(mousePos, _camera)};

			//if pixel pos outside of object e on x axis
			if (!(pixelPos.x > cT.pos.x - eBB.halfSize.x && pixelPos.x < cT.pos.x + eBB.halfSize.x))
				continue;
			
			//if pixel pos is outside of object e on y axis
			if (!(pixelPos.y > cT.pos.y - eBB.halfSize.y && pixelPos.y < cT.pos.y + eBB.halfSize.y))
				continue;

			if(selectedEntity != nullptr)
				selectedEntity->getComponent<CBoundingBox>().selected = 0;

			//if pixel pos is within the object
			selectedEntity = e;

			selectedEntity->getComponent<CBoundingBox>().selected = 1;
		}
	}

	if (action.name() == "MouseWheel" && !ImGui::GetIO().WantCaptureMouse)
	{
		if(action.mouseWheelDelta != 0)
		{
			float zoom = action.mouseWheelDelta > 0 ? 0.9f : 10.0f/9.0f;

			const Vector2 beforeCoord = { GetScreenToWorld2D(Vector2(action.mouseX, action.mouseY), _camera) };
			_camera.zoom *= zoom;

			const Vector2 afterCoord = { GetScreenToWorld2D(Vector2(action.mouseX, action.mouseY), _camera) };

			//_view.move(beforeCoord - afterCoord);
			_camera.offset.x -= (beforeCoord.x - afterCoord.x) * _camera.zoom;
			_camera.offset.y -= (beforeCoord.y - afterCoord.y) * _camera.zoom;

		}
	}
	
	if (action.name() == "MiddleClick" && action.type() == "START" && !ImGui::GetIO().WantCaptureMouse)
	{
		_tagMenu.panningFlag = true;
	}
	if (action.name() == "MiddleClick" && action.type() == "END")
	{
		_tagMenu.panningFlag = false;
	}
}


void PLevelEditor::saveSprite(Sprite& sprite)
{
	std::string spriteName = "sprite_" + std::to_string(_sprites.size());
	
	_sprites[spriteName] = sprite;
}



void PLevelEditor::sRender()
{
	Color color = { 137, 207, 240, 255 };

	BeginDrawing();
	ClearBackground(color);

	gridToggle(64, false);

	BeginMode2D(_camera);

	//A VERY quick rendering scheme for testing, needs to change
	for (auto& e : _entities.getEntities())
	{
		if (e->getComponent<CTransform>().has)
		{
			Rectangle position = e->getComponent<CSprite>().textureRect;
			position.x = e->getComponent<CTransform>().pos.x;
			position.y = e->getComponent<CTransform>().pos.y;

			DrawTexturePro(e->getComponent<CSprite>().texture, e->getComponent<CSprite>().textureRect, position,
				e->getComponent<CSprite>().origin, 0, RAYWHITE);
		}
	}
	sBBRender();

	EndMode2D();

	rlImGuiBegin();

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
	ImGui::ShowDemoWindow();
	rlImGuiEnd();

	EndDrawing();
}



/*
	Draws a square grid with [gridSize] pixels spacing.
	aligned to either bottom-left or top-left
*/
void PLevelEditor::gridToggle(int gridSize, bool alignBottomLeft)
{
	//for (std::shared_ptr<Entity> e : _entities.getEntities())
	//{
	//	if (e->getComponent<CBoundingBox>().has)
	//	{
	//		Vec2 bbSize = e->getComponent<CBoundingBox>().size;
	//		Vec2 bbPos = e->getComponent<CTransform>().pos;

	//		DrawRectangleLines(bbPos.x - bbSize.x / 2, bbPos.y - bbSize.y / 2, bbSize.x, bbSize.y, RED);
	//	}
	//}
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
			cfgFile << IO::formatComponent(cT);
		}

		if (e->getComponent<CBoundingBox>().has)
		{
			auto& cBB = e->getComponent<CBoundingBox>();
			cfgFile << IO::formatComponent(cBB);
		}

		if (e->getComponent<CSprite>().has)
		{
			auto& cS = e->getComponent<CSprite>();
			std::string textureName;
			for (auto& [k, v] : _game->getAssets().getTextures())
			{
				if (&cS.texture == &v)
				{
					cfgFile << IO::formatComponent(cS, k);
					break;
				}
			}
		}

		cfgFile << "END" << std::endl;
	}
	cfgFile.close();
}

void PLevelEditor::sBBRender()
{
	for (std::shared_ptr<Entity> e : _entities.getEntities())
	{
		if (e->getComponent<CBoundingBox>().has)
		{
			Vec2 bbSize = e->getComponent<CBoundingBox>().size;
			Vec2 bbPos = e->getComponent<CTransform>().pos;

			DrawRectangleLines(bbPos.x - bbSize.x / 2, bbPos.y - bbSize.y / 2, bbSize.x, bbSize.y, RED);
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
		saveLevel("./cfgTemp.cfg");
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
								obj->getComponent<CBoundingBox>().selected = 0;
							}
							e->getComponent<CBoundingBox>().selected = 1;
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
static Vector2 spriteResize(const Rectangle& spriteSize, int maxSize)
{
	Vector2 imageSize = { 0.0f, 0.0f };
	float sizeRatio = spriteSize.width / spriteSize.height;

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

	ImGui::Text("Add a new entity:");
	ImGui::InputText("Entity_tag", _newEntityTag, 20);

	if (ImGui::Button("Create"))
	{
		if (selectedEntity)
			selectedEntity->getComponent<CBoundingBox>().selected = false;

		selectedEntity = _entities.addEntity(std::string(_newEntityTag));

		selectedEntity->addComponent<CBoundingBox>(Vec2(32, 32));
		selectedEntity->getComponent<CBoundingBox>().selected = true;

	}

	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	if (!selectedEntity)
	{
		ImGui::TextColored(_imGuiVars.warningColor, "Please select an entity.");
		ImGui::End();
		return;
	}

	std::shared_ptr<Entity> entity = selectedEntity;

	if (entity->getComponent<CSprite>().has)
	{
		Rectangle spriteSize = entity->getComponent<CSprite>().textureRect;

		Vector2 imageSize = spriteResize(spriteSize, 100);

		/*ImGui::Image(selectedEntity->getComponent<CSprite>().sprite,
			imageSize, sf::Color::White, sf::Color::White);*/

		rlImGuiImageRect(&selectedEntity->getComponent<CSprite>().texture, imageSize.x, imageSize.y, spriteSize);

		ImGui::SameLine();
	}
	
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
							_spriteRect = _selectedSprite.textureRect;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				rlImGuiImageRect(&_selectedSprite.texture, spriteResize(_selectedSprite.textureRect, 50).x, 
					spriteResize(_selectedSprite.textureRect, 50).y, _selectedSprite.textureRect);

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

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
	if (ImGui::Button("Delete!"))
	{
		selectedEntity->destroy();
		selectedEntity = nullptr;
	}
	ImGui::PopStyleColor();

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

		Vector2 texSize = { _game->getAssets().getTexture(_selectedTexture).width, _game->getAssets().getTexture(_selectedTexture).height };

		if (resetFlag) //this resets the displayed texture rect to the whole texture
		{
			_selectedSprite = Sprite(_game->getAssets().getTexture(_selectedTexture));
			_spriteRect = Rectangle(0, 0, texSize.x, texSize.y);
			resetFlag = 0;
		}

		Vector2 imageSize = spriteResize(_spriteRect, 400);

		// Show selected texture in a window
		// selected texture is initialized in the init method
		//ImGui::Image(_selectedSprite, imageSize, sf::Color::White, sf::Color::White);

		rlImGuiImageRect(&_selectedSprite.texture, imageSize.x,
			imageSize.y, _selectedSprite.textureRect);

		//create a menu that allows to change the rect of the sprite
		ImGui::SetCursorPosY(500);

		ImGui::PushItemWidth(150);
		ImGui::SliderFloat("##X pos", &_spriteRect.x, 0, texSize.x-1);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
		{ _spriteRect.x = MATH::clamp(0, _spriteRect.x-1, texSize.x-1); }
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
		{ _spriteRect.x = MATH::clamp(0, _spriteRect.x + 1, texSize.x - 1); }
		ImGui::SameLine();
		ImGui::Text("Left");
		ImGui::SameLine(0.0f, 40.0f);

		ImGui::PushItemWidth(150);
		ImGui::SliderFloat("##Y pos", &_spriteRect.y, 0, texSize.y-1);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##top", ImGuiDir_Left)) 
		{ _spriteRect.y = MATH::clamp(0, _spriteRect.y - 1, texSize.y - 1); }
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##down", ImGuiDir_Right)) 
		{ _spriteRect.y = MATH::clamp(0, _spriteRect.y + 1, texSize.y - 1); }
		ImGui::SameLine();
		ImGui::Text("Top");
		
		ImGui::PushItemWidth(150);
		ImGui::SliderFloat("##width", &_spriteRect.width, 0, texSize.x-1);
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
		ImGui::SliderFloat("##height", &_spriteRect.height, 0, texSize.y-1);
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##height-", ImGuiDir_Left)) 
		{ _spriteRect.height = MATH::clamp(0, _spriteRect.height - 1, texSize.y - 1); }
		ImGui::SameLine(0.0f, 1.0f);
		if (ImGui::ArrowButton("##height+", ImGuiDir_Right)) 
		{ _spriteRect.height = MATH::clamp(0, _spriteRect.height + 1, texSize.y - 1); }
		ImGui::SameLine();
		ImGui::Text("Height");

		_selectedSprite.textureRect = _spriteRect;

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
			ImGui::TextColored(_imGuiVars.messageColor, "Saved!");
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
					Rectangle texRect;

					fin >> textureName;

					//if texture for a sprite is not loaded into the memory, load it
					if (_game->getAssets().getTextures().find(textureName) == _game->getAssets().getTextures().end())
					{
						_game->getAssets().addTexture(textureName, _textureData.texturePath + textureName);
					}
						
					fin >> texRect.x >> texRect.y >> texRect.width >> texRect.height;

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