#include "textureEditWindow.h"
#include "../../Utils/messages.h"
#include "../../Utils/mathStuff.h"

void TextureEditWindow::useTexture(sf::Texture& texture)
{
	_sprite.setTexture(texture);
}

void TextureEditWindow::setBackground()
{
	background.setPosition(32, 32);
	background.setSize(sf::Vector2f(_size.x, _size.y));
	background.setScale(backgroundScale, backgroundScale);
	background.setOutlineColor(sf::Color::Red);
	background.setOutlineThickness(5);
}

void TextureEditWindow::scaleToMainWindow(sf::Vector2u windowSize)
{
	_size = _sprite.getTexture()->getSize();
	_sprite.setPosition(32, 32);
	textureRect = sf::IntRect(0, 0, _size.x, _size.y);

	/* scale to the larger of the axis to fit the contents in the window */
	if (_size.x > _size.y)
	{
		backgroundScale = (float)(windowSize.x - 64) / _size.x;
	}
	else
	{
		backgroundScale = (float)(windowSize.y - 64) / _size.y;
	}
	_sprite.setScale(backgroundScale, backgroundScale);
}

void TextureEditWindow::setPanning(sf::Vector2i mousePos)
{
	mouseCoordsInTexture = coordChange(mousePos);
	mouseState = mS_PANNING;
}

void TextureEditWindow::stopPanning()
{
	mouseState = mS_DEFAULT;
}

void TextureEditWindow::pan(int mousePosX, int mousePosY)
{
	if (mouseState != mS_PANNING) { return; };
	
	sf::Vector2i currentCoordTexture = coordChange(sf::Vector2i(mousePosX, mousePosY));
	if (currentCoordTexture.x == -1 && currentCoordTexture.y == -1)
	{
		return;
	}

	currentCoordTexture = mouseCoordsInTexture - currentCoordTexture;

	textureRect.left += currentCoordTexture.x;
	textureRect.top += currentCoordTexture.y;
}

void TextureEditWindow::startSelection(sf::Vector2i mousePos)
{
	mouseCoords = mousePos;
	mouseState = mS_SELECTION;
}

void TextureEditWindow::selection(int mousePosX, int mousePosY)
{
	if (mouseState != mS_SELECTION) { return; };

	sf::Vector2i mousePos = relativeMousePos(sf::Vector2u(1280, 720), mouseCoords.x, mouseCoords.y);
	if (mousePos.x == -1 && mousePos.y == -1) { return; };

	sf::Color translucent(0, 0, 0, 0);
	sf::Color redOutline(255, 0, 0, 255);

	selection_.setPosition(mousePos.x + 32, mousePos.y + 32);

	selection_.setFillColor(translucent);
	selection_.setOutlineColor(redOutline);
	selection_.setOutlineThickness(1);

	sf::Vector2i nextMousePos = relativeMousePos(sf::Vector2u(1280, 720), mousePosX, mousePosY);
	if (nextMousePos.x == -1 && nextMousePos.y == -1) { return; };

	rectWidth = nextMousePos.x - mousePos.x;
	rectHeight = nextMousePos.y - mousePos.y;
	selection_.setSize(sf::Vector2f(rectWidth, rectHeight));
}

sf::IntRect TextureEditWindow::stopSelection()
{
	mouseState = mS_DEFAULT;
	

	sf::Vector2i selectionPos = coordChange(sf::Vector2i((int)selection_.getPosition().x, (int)selection_.getPosition().y));
	sf::Vector2i selectionSize = sf::Vector2i((int)(selection_.getSize().x / backgroundScale*relativeScale), (int)(selection_.getSize().y / backgroundScale/relativeScale));

	sf::IntRect selectionInTexture(selectionPos.x, selectionPos.y, selectionSize.x, selectionSize.y);

	return selectionInTexture;
}

sf::Vector2i TextureEditWindow::coordChange(sf::Vector2i pointPos)
{
	sf::Vector2i newPointPos(0, 0);
	sf::Vector2i pointInBackground = relativeMousePos(sf::Vector2u(1280, 720), pointPos.x, pointPos.y);
	if (pointInBackground.x == -1 && pointInBackground.y == -1)
	{
		MSG::WARNING("Outside of texture background.");
		return pointInBackground;
	}

	if (relativeScale < 1)
	{
		sf::Vector2f spritePos = _sprite.getPosition();
		newPointPos.x = (pointInBackground.x - spritePos.x + 32) / (relativeScale * backgroundScale);
		newPointPos.y = (pointInBackground.y - spritePos.y + 32) / (relativeScale * backgroundScale);
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
	//calculate mouse position relative to the rectangle window
	sf::Vector2i relMouPos = relativeMousePos(sf::Vector2u(1280, 720), mousePosX, mousePosY);
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

const sf::Vector2i TextureEditWindow::relativeMousePos(sf::Vector2u windowSize, int mousePosX, int mousePosY) const
{
	sf::Vector2i mouseShifted;
	mouseShifted.x = mousePosX - 32;
	mouseShifted.y = mousePosY - 32;

	if (mouseShifted.x >= 0 && mouseShifted.y >= 0)
	{
		if (mouseShifted.x <= (_size.x*backgroundScale) && mouseShifted.y <= (_size.y*backgroundScale))
		{
			return mouseShifted;
		}
	}
	return sf::Vector2i(-1, -1);
}

sf::Sprite& TextureEditWindow::getTexture()
{
	_sprite.setTextureRect(textureRect);
	return _sprite;
}