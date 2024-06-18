#include "textureEditWindow.h"
#include "../../../Utils/messages.h"
#include "../../../Utils/mathStuff.h"



void TextureEditWindow::useTexture(sf::Texture& texture)
{
	_sprite.setTexture(texture);

	_size = _sprite.getTexture()->getSize();
	_sprite.setPosition(_position.x, _position.y);
	textureRect = sf::IntRect(0, 0, _size.x, _size.y);

	/* scale to the larger of the axis to fit the contents in the game window */
	if (_size.x > _size.y)
	{
		backgroundScale = (float)(_gameWindowSize.x - _position.x*2) / _size.x;
	}
	else
	{
		backgroundScale = (float)(_gameWindowSize.y - _position.y*2) / _size.y;
	}
	_sprite.setScale(backgroundScale, backgroundScale);
}



void TextureEditWindow::setPanning(const sf::Vector2i& mousePos)
{
	_mouseCoordsInTexture = coordChange(mousePos);
	mouseState = mS_PANNING;
}



void TextureEditWindow::stopPanning()
{
	mouseState = mS_DEFAULT;
}



void TextureEditWindow::pan(const sf::Vector2i& mousePosition)
{	
	sf::Vector2i currentCoordTexture = coordChange(mousePosition);
	if (currentCoordTexture.x == -1 && currentCoordTexture.y == -1)
	{
		return;
	}

	currentCoordTexture = _mouseCoordsInTexture - currentCoordTexture;

	textureRect.left += currentCoordTexture.x;
	textureRect.top += currentCoordTexture.y;
}



void TextureEditWindow::startSelection(const sf::Vector2i& mousePos)
{
	_mouseCoords = relativeMousePos(mousePos.x, mousePos.y);
	if (_mouseCoords.x == -1 && _mouseCoords.y == -1) { return; };
	_mouseCoords.x += _position.x;
	_mouseCoords.y += _position.y;
	mouseState = mS_SELECTION;
}



void TextureEditWindow::selection(const sf::Vector2i& mousePosition)
{
	//mouse position when the selection was started
	sf::Vector2i mousePos = relativeMousePos(_mouseCoords.x, _mouseCoords.y);
	if (mousePos.x == -1 && mousePos.y == -1) { return; };

	//current mouse position
	sf::Vector2i nextMousePos = relativeMousePos(mousePosition.x, mousePosition.y);
	if (nextMousePos.x == -1 && nextMousePos.y == -1) { return; };
	
	_rectWidth = nextMousePos.x - mousePos.x;
	_rectHeight = nextMousePos.y - mousePos.y;

	//if the selection goes from (top-left to bottom-right) it works great
	//but if it goes in one of the other 3 directions it will break
	//so it needs to be corrected as below
	if (nextMousePos.x < mousePos.x)
	{
		mousePos.x = nextMousePos.x;
		_rectWidth *= -1;
	}
	if (nextMousePos.y < mousePos.y)
	{
		mousePos.y = nextMousePos.y;
		_rectHeight *= -1;
	}

	_selection.setPosition(mousePos.x + _position.x, mousePos.y + _position.y);
	_selection.setSize(sf::Vector2f(_rectWidth, _rectHeight));

	_selection.setFillColor(sf::Color::Transparent);
	_selection.setOutlineColor(sf::Color::Black);
	_selection.setOutlineThickness(1);
}



sf::IntRect TextureEditWindow::stopSelection()
{
	mouseState = mS_DEFAULT;

	sf::Vector2i selectionPos = coordChange(sf::Vector2i(_selection.getPosition()));
	
	int selectionSizeX = (int)(_selection.getSize().x / (backgroundScale * relativeScale));
	int selectionSizeY = (int)(_selection.getSize().y / (backgroundScale * relativeScale));

	sf::IntRect selectionInTexture(selectionPos.x, selectionPos.y, selectionSizeX, selectionSizeY);
	
	return selectionInTexture;
}



sf::Vector2i TextureEditWindow::coordChange(const sf::Vector2i& pointPos)
{
	sf::Vector2i newPointPos(0, 0);
	sf::Vector2i pointInBackground = relativeMousePos(pointPos.x, pointPos.y);
	if (pointInBackground.x == -1 && pointInBackground.y == -1)
	{
		MSG::WARNING("Outside of texture background.");
		return pointInBackground;
	}

	if (relativeScale < 1)
	{
		sf::Vector2f spritePos = _sprite.getPosition();
		newPointPos.x = (pointInBackground.x - spritePos.x + _position.x) / (relativeScale * backgroundScale);
		newPointPos.y = (pointInBackground.y - spritePos.y + _position.y) / (relativeScale * backgroundScale);
	}
	else if (relativeScale >= 1)
	{
		int posX = pointInBackground.x / (relativeScale * backgroundScale);
		int posY = pointInBackground.y / (relativeScale * backgroundScale);
		newPointPos.x = posX + textureRect.left;
		newPointPos.y = posY + textureRect.top;
	}
	else
	{
		MSG::ERROR("Something went wrong in TextureEditWindow::coordChange()");
		MSG::ERROR("Relative Scale: ", relativeScale);
	}

	return newPointPos;
}



void TextureEditWindow::zoom(int ticks, int mousePosX, int mousePosY)
{
	if (mouseState == mS_SELECTION) { return; };

	//calculate mouse position relative to the rectangle window
	sf::Vector2i relMouPos = relativeMousePos(mousePosX, mousePosY);
	if (relMouPos.x == -1 && relMouPos.y == -1) { return; };

	// get the position of the mouse on the texture before the scaling
	// (to be able to zoom to the mouse cursor)
	sf::Vector2i mouseOnTexture = coordChange(sf::Vector2i(mousePosX, mousePosY));

	/* 1.25^(ticks) or 1.25^(-ticks) (in case of 1 tick = 0.8)
	   0.8 * 1.25 = 1 so the scale will always return to 1 */
	relativeScale = relativeScale * (pow(1.25f, ticks));

	/* clamp the value to min and max
	   (cant use absolute values, must be a value on the function
	   to avoid shifting of the scale on the boundries) */
	relativeScale = MATH::clamp(1, relativeScale, (1 * pow(1.25f, 15)));

	/* set the scale of the texture sprite relative to the window */
	_sprite.setScale(relativeScale * backgroundScale, relativeScale * backgroundScale);

	float zoomRectWidth = MATH::clamp(1, _size.x / relativeScale, _size.x);
	float zoomRectHeight = MATH::clamp(1, _size.y / relativeScale, _size.y);
	textureRect.height = zoomRectHeight;
	textureRect.width = zoomRectWidth;

	// get the position of the mouse on the texture after the scaling
	// and move the TextureRect accordingly
	sf::Vector2i mouseOnTextureAfterScale = coordChange(sf::Vector2i(mousePosX, mousePosY));

	if (relativeScale <= 1)
	{
		textureRect.top = 0;
		textureRect.left = 0;
	}
	else if (relativeScale > 1)
	{
		textureRect.left += mouseOnTexture.x - mouseOnTextureAfterScale.x;
		textureRect.top += mouseOnTexture.y - mouseOnTextureAfterScale.y;
	}
}



const sf::Vector2i TextureEditWindow::relativeMousePos(int mousePosX, int mousePosY) const
{
	sf::Vector2i mouseShifted;
	mouseShifted.x = mousePosX - _position.x;
	mouseShifted.y = mousePosY - _position.y;

	if (mouseShifted.x >= 0 && mouseShifted.y >= 0)
	{
		if (mouseShifted.x <= (_size.x*backgroundScale) && mouseShifted.y <= (_size.y*backgroundScale))
		{
			return mouseShifted;
		}
	}
	return sf::Vector2i(-1, -1);
}



void TextureEditWindow::sWindowRender(sf::RenderWindow& window)
{
	background.setPosition(_position.x, _position.y);
	background.setSize(sf::Vector2f(_size.x, _size.y));
	background.setScale(backgroundScale, backgroundScale);
	background.setOutlineColor(sf::Color::Red);
	background.setOutlineThickness(5);

	if(mouseState == mS_PANNING)
		pan(sf::Mouse::getPosition(window));

	if(mouseState == mS_SELECTION)
		selection(sf::Mouse::getPosition(window));

	_sprite.setTextureRect(textureRect);
	window.draw(background);
	window.draw(_sprite);

	if (mouseState == mS_SELECTION)
	{
		window.draw(_selection);
	}
}



sf::Sprite& TextureEditWindow::getTexture()
{
	return _sprite;
}