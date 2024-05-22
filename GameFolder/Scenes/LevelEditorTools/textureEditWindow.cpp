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

void TextureEditWindow::pan(int mousePosX, int mousePosY, sf::Vector2i coordsInTexture)
{
	
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

	if (relativeScale == 1)
	{
		newPointPos.x = pointInBackground.x / (relativeScale * backgroundScale);
		newPointPos.y = pointInBackground.y / (relativeScale * backgroundScale);
	}
	else if (relativeScale < 1)
	{
		sf::Vector2f spritePos = _sprite.getPosition();
		newPointPos.x = (pointInBackground.x - spritePos.x + 32) / (relativeScale * backgroundScale);
		newPointPos.y = (pointInBackground.y - spritePos.y + 32) / (relativeScale * backgroundScale);
	}
	else if (relativeScale > 1)
	{
		sf::IntRect textRect = _sprite.getTextureRect();
		int posX = pointInBackground.x / (relativeScale * backgroundScale);
		int posY = pointInBackground.y / (relativeScale * backgroundScale);
		newPointPos.x = posX + textRect.getPosition().x;
		newPointPos.y = posY + textRect.getPosition().y;
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
	MSG::TRACE("Mouse before scale: ", mouseOnTexture.x, " : ", mouseOnTexture.y);

	/* 1.25^(ticks) or 1.25^(-ticks) (in case of 1 tick = 0.8)
	   0.8 * 1.25 = 1 so the scale will always return to 1 */
	relativeScale = relativeScale * (pow(1.25f, ticks));

	/* clamp the value to min and max 
	   (cant use absolute values, must be a value on the function
	   to avoid shifting of the scale on the boundries) */
	relativeScale = MATH::clamp( (1 * pow(1.25f, -10)), relativeScale, (1* pow(1.25f, 15)) );

	/* set the scale of the texture sprite relative to the window */
	_sprite.setScale(relativeScale*backgroundScale, relativeScale*backgroundScale);

	// get the position of the mouse on the texture after the scaling
	// and move the TextureRect (or sprite if the image is zoomed out) accordingly
	sf::Vector2i mouseOnTextureAfterScale = coordChange(sf::Vector2i(mousePosX, mousePosY));
	MSG::TRACE("Mouse after scale: ", mouseOnTextureAfterScale.x, " : ", mouseOnTextureAfterScale.y);
	static int beginX = 0;
	static int beginY = 0;
	if (relativeScale <= 1)
	{
		beginX = 0;
		beginY = 0;
	}
	else if (relativeScale > 1)
	{
		beginX += mouseOnTexture.x - mouseOnTextureAfterScale.x;
		beginY += mouseOnTexture.y - mouseOnTextureAfterScale.y;
	}

	/* set the zoomed in subrectangle of the texture
	   make sure it is within the bounds of the texture */
	float zoomRectWidth = MATH::clamp(1, _size.x / relativeScale, _size.x);
	float zoomRectHeight = MATH::clamp(1, _size.y / relativeScale, _size.y);
	sf::IntRect rect(beginX, beginY, zoomRectWidth, zoomRectHeight);

	MSG::TRACE("Sprite pos: ", _sprite.getPosition().x, " : ", _sprite.getPosition().y);

	_sprite.setTextureRect(rect);
}

const sf::Vector2i TextureEditWindow::relativeMousePos(sf::Vector2u windowSize, int mousePosX, int mousePosY) const
{
	sf::Vector2i mouseShifted;
	mouseShifted.x = mousePosX - 32;
	mouseShifted.y = mousePosY - 32;

	if (mouseShifted.x > 0 && mouseShifted.y > 0)
	{
		if (mouseShifted.x < (_size.x*backgroundScale) && mouseShifted.y < (_size.y*backgroundScale))
		{
			return mouseShifted;
		}
	}
	return sf::Vector2i(-1, -1);
}

sf::Sprite TextureEditWindow::getTexture()
{
	_sprite.setPosition(32, 32);
	return _sprite;
}