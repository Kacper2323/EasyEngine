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


void TextureEditWindow::zoom(int ticks, int mousePosX, int mousePosY)
{
	//calculate mouse position relative to the rectangle window
	sf::Vector2i relMouPos = relativeMousePos(sf::Vector2u(1280, 720), sf::Vector2i(mousePosX, mousePosY));
	if (relMouPos.x == 0 && relMouPos.y == 0) { return; };

	/* 1.25^(ticks) or 1.25^(-ticks) (in case of 1 tick = 0.8)
	   0.8 * 1.25 = 1 so the scale will always return to 1 */
	relativeScale = relativeScale * (pow(1.25f, ticks));

	/* clamp the value to min and max 
	   (cant use literal values, must be a value on the function
	   to avoid shifting of the scale on the boundries) */
	relativeScale = MATH::clamp( (1 * pow(1.25f, -10)), relativeScale, (1* pow(1.25f, 10)) );

	MSG::TRACE("Scale: ", relativeScale);

	/* set the scale of the texture sprite relative to the window */
	_sprite.setScale(relativeScale*backgroundScale, relativeScale*backgroundScale);

	//calculate the position within the texture

	//pick the starting point of the texture rectangle according to the mouse (to keep the mouse in the middle)

	/* set the zoomed in subrectangle of the texture
	   make sure it is within the bounds of the texture */
	float zoomRectWidth = MATH::clamp(1, _size.x / relativeScale, _size.x);
	float zoomRectHeight = MATH::clamp(1, _size.y / relativeScale, _size.y);
	sf::IntRect rect(0, 0, zoomRectWidth, zoomRectHeight);

	MSG::TRACE("Width: ", zoomRectWidth);
	MSG::TRACE("Height: ", zoomRectHeight);
	_sprite.setTextureRect(rect);
}

const sf::Vector2i TextureEditWindow::relativeMousePos(sf::Vector2u windowSize, sf::Vector2i mousePos) const
{
	sf::Vector2i mouseShifted;
	mouseShifted.x = mousePos.x - 32;
	mouseShifted.y = mousePos.y - 32;

	if (mouseShifted.x > 0 && mouseShifted.y > 0)
	{
		if (mouseShifted.x < (_size.x*backgroundScale) && mouseShifted.y < (_size.y*backgroundScale))
		{
			return mouseShifted;
		}
	}
	return sf::Vector2i(0, 0);
}

sf::Sprite TextureEditWindow::getTexture()
{
	_sprite.setPosition(32, 32);
	return _sprite;
}