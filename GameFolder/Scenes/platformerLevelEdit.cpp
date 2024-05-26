#include "platformerLevelEdit.h"
#include "../Utils/messages.h"

PLevelEditor::PLevelEditor() {};

PLevelEditor::PLevelEditor(GameEngine* gameEnginePointer)
	:Scene(gameEnginePointer)
{
	registerAction(sf::Mouse::Left + mouseButtonOFFSET, "LeftClick");
	registerAction(sf::Mouse::Right + mouseButtonOFFSET, "RightClick");
	registerAction(sf::Mouse::Middle + mouseButtonOFFSET, "MiddleClick");
	registerAction(mouseWheelOFFSET, "MouseWheel");
	init();
};

void PLevelEditor::init()
{
	//initialize a list of available textures in textureData
	textureData.texturePath = "./GameFolder/Assets/Textures/";
	std::string fileName;
	for(const auto& entry : std::filesystem::directory_iterator(textureData.texturePath))
	{
		fileName = entry.path().filename().string();

		std::cout << fileName << std::endl;
		textureData.textureNames.push_back(fileName);
	}
	imGuiBinds.current_obj = &textureData.textureNames[0];
}

void PLevelEditor::update()
{
	_entities.Update();
	ImGui::SFML::Update(_game->_window, deltaClock.restart());
	ImGui::ShowDemoWindow();
}

void PLevelEditor::sDoAction(Action action)
{
	if (action.name() == "LeftClick" && action.type() == "START")
	{
		if (imGuiBinds.showTextureWindow)
		{
			editTexture.setPanning(sf::Vector2i(action.mouseX, action.mouseY));
		}
	}
	if (action.name() == "LeftClick" && action.type() == "END")
	{
		if (imGuiBinds.showTextureWindow)
		{
			editTexture.stopPanning();
		}
	}
	if (action.name() == "MouseWheel")
	{
		if (imGuiBinds.showTextureWindow)
		{
			editTexture.zoom(action.mouseWheelDelta, action.mouseX, action.mouseY);
		}
	}
	if (action.name() == "RightClick" && action.type() == "START")
	{
		MSG::TRACE("Selection...");
		editTexture.startSelection(sf::Mouse::getPosition(_game->_window));
	}
	if (action.name() == "RightClick" && action.type() == "END")
	{
		MSG::TRACE("Stop selection.");
		editTexture.stopSelection();
	}
}

void PLevelEditor::sRender()
{
	_game->_window.clear(imGuiBinds.background);
	gridToggle(64, false);


	if (imGuiBinds.showTextureWindow)
	{
		editTexture.pan(sf::Mouse::getPosition(_game->_window).x, sf::Mouse::getPosition(_game->_window).y);
		textureWindow();
		editTexture.selection(sf::Mouse::getPosition(_game->_window).x, sf::Mouse::getPosition(_game->_window).y);
		if (editTexture.mouseState == mS_SELECTION)
		{
			_game->_window.draw(editTexture.selection_);
		}
	}
	ImGuiRender();

	
	ImGui::SFML::Render(_game->_window);
	_game->_window.display();
}

/*
	Draws a square grid with [gridSize] pixels spacing.
	aligned to either bottom-left or top-left
*/
void PLevelEditor::gridToggle(int gridSize, bool alignBottomLeft)
{
	sf::Vector2u windowSize = _game->_window.getSize();

	sf::Vertex line[2];

	for (int i = 1; i < windowSize.x; i += gridSize)
	{
		line[0].position = sf::Vector2f(i, 0);
		line[0].color = sf::Color::White;
		line[1].position = sf::Vector2f(i, windowSize.y);
		line[1].color = sf::Color::White;
		_game->_window.draw(line, 2, sf::Lines);
	}

	if (alignBottomLeft)
	{
		for (int i = windowSize.y; i > 0; i -= gridSize)
		{
			line[0].position = sf::Vector2f(0, i);
			line[0].color = sf::Color::White;
			line[1].position = sf::Vector2f(windowSize.x, i);
			line[1].color = sf::Color::White;
			_game->_window.draw(line, 2, sf::Lines);
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
			_game->_window.draw(line, 2, sf::Lines);
		}
	}
}

void PLevelEditor::ImGuiRender()
{
	//imGuiBinds.current_obj = textureData.textureNames[0];

	ImGui::Begin("Window title");

	ImGui::Text("Texture loader");
	if (ImGui::BeginCombo("##texture_loader",  imGuiBinds.current_obj->c_str()))
	{
		for (std::string& name : textureData.textureNames)
		{
			bool is_selected = (&name == imGuiBinds.current_obj);
			if (ImGui::Selectable(name.c_str(), is_selected))
			{
				imGuiBinds.current_obj = &name;
				imGuiBinds.showTextureWindow = false;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	if (editorAssets.getTextures().find(*imGuiBinds.current_obj) == editorAssets.getTextures().end())
	{
		if (ImGui::Button("load texture")) {
			editorAssets.addTexture(*imGuiBinds.current_obj, textureData.texturePath + *imGuiBinds.current_obj);
		}
	}
	else
	{
		ImGui::Text("Texture ready");
	}

	test();

	ImGui::End();
}

// draw a window with selected texture
// scale it to the window
// allow for zooming in and out
// allow for panning
// allow for rectangular selection of a subregion
// extract position and size of selection relative to texture window

void PLevelEditor::test()
{
	if (!imGuiBinds.showTextureWindow)
	{
		if (ImGui::Button("show texture"))
		{
			if (imGuiBinds.current_obj == nullptr)
			{
				MSG::ERROR("current_obj in imGuiBinds is a nullptr");
				imGuiBinds.showTextureWindow = false;
				return;
			}
			if (editorAssets.getTextures().find(*imGuiBinds.current_obj) == editorAssets.getTextures().end())
			{
				MSG::ERROR(*imGuiBinds.current_obj, "in current_obj is not loaded into editors assets");
				imGuiBinds.showTextureWindow = false;
				return;
			}
			editTexture.useTexture(editorAssets.getTexture(*imGuiBinds.current_obj));
			editTexture.scaleToMainWindow(_game->_window.getSize());
			imGuiBinds.showTextureWindow = true;
		}
	}
	else
	{
		if (ImGui::Button("hide texture"))
		{
			imGuiBinds.showTextureWindow = false;
		}
	}
}

void PLevelEditor::textureWindow()
{
	
	editTexture.setBackground();
	_game->_window.draw(editTexture.background);
	_game->_window.draw(editTexture.getTexture());
}

