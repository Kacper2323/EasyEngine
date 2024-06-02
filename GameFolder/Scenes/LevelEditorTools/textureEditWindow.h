#pragma once

#include <SFML/Graphics.hpp>
#include "../../Utils/messages.h"
#include "../../Vec2.h"

enum mouseState
{
	mS_DEFAULT,
	mS_PANNING,
	mS_SELECTION
};

/*
Texture edit window class.
Allows to pick subtextures from a single texture to then use as sprites.
*/
class TextureEditWindow
{
	// the class started growing a little with different functionalities 
	// so I divided it into sections

///////////////////////////////////////////////////////////////////////////////////
/*
General section.
This section contains general variables and functions necessary for the class.
*/
///////////////////////////////////////////////////////////////////////////////////
private:

	sf::Sprite _sprite;	//the texture to be displayed within the texture window
	sf::IntRect textureRect;	//subrectangle of the texture to be displayed
	mouseState mouseState = mS_DEFAULT;
	const sf::Vector2u _gameWindowSize;

	sf::RectangleShape background;
	sf::Vector2u _size;	//size of the texture
	sf::Vector2u _position = {32, 32};	//position of the texture window (from top,left)
	float backgroundScale = 1;	//scale of the background
	float relativeScale = 1;	//scale of the texture relative to background

public:

	TextureEditWindow() {};
	TextureEditWindow(const sf::Vector2u& gameWindowSize)
		:_gameWindowSize(gameWindowSize)
	{};

	/*
	Set the texture to be displaye.
	\param texture: referance to sf::Texture to be displayed
	*/
	void useTexture(sf::Texture& texture);

	/*
	Rendering system for the texture window.
	\param window: game window to render to
	*/
	void sWindowRender(sf::RenderWindow& window);

	/*
	Zoom the texture in and out within the background.
	\param ticks: ticks of mouse wheel (positive if up, negative if down)
	\param mousePos: X and Y coordinates (within the rendering window) 
	of the mouse at the moment of scrolling
	*/
	void zoom(int ticks, int mousePosX, int mouosPosY);

///////////////////////////////////////////////////////////////////////////////////
/*
Panning section.
This section contains variables and functions for the panning functionality.
*/
///////////////////////////////////////////////////////////////////////////////////
private:

	sf::Vector2i _mouseCoordsInTexture;	//mouse position in coords of the texture 
										//at the moment panning was started

	/*
	Pan the texture within the background.
	\param mousePosX: X coordinate of the mouse position
	\param mousePosY: Y coordinate of the mouse position
	*/
	void pan(const sf::Vector2i& mousePosition);

public:

	/*
	Set panning. Causes the texture to move with the mouse.
	\param mousePosX: X coordinate of the mouse position
	\param mousePosY: Y coordinate of the mouse position
	*/
	void setPanning(const sf::Vector2i& coordsInTexture);

	/*
	Stop panning. Causes the texture to stop moving with the mouse.
	*/
	void stopPanning();

///////////////////////////////////////////////////////////////////////////////////
/*
Selection section.
This section contains variables and functions for the selection mechanism.
*/
///////////////////////////////////////////////////////////////////////////////////
private:

	sf::Vector2i _mouseCoords;	//mouse position at the moment selection was started (beginning of selection)
	int _rectWidth = 0;
	int _rectHeight = 0;
	sf::RectangleShape _selection;

	/*
	Selection system. Resizes the selection to the position of the mouse.
	\param mousePosX: X coordinate of the mouse position
	\param mousePosY: Y coordinate of the mouse position
	*/
	void selection(const sf::Vector2i& mousePosiotion);

public:

	/*
	Start a selection rectangle on the texture window.
	\param mousePos: mouse position in the game window
	*/
	void startSelection(const sf::Vector2i& mousePos);

	/*
	Stop the selection.
	\return selection coordinates (left, top, width, height)
	*/
	sf::IntRect stopSelection();

///////////////////////////////////////////////////////////////////////////////////
/*
Utils section.
*/
///////////////////////////////////////////////////////////////////////////////////
private:

	/*
	Calculates the coordinates of a point on a texture based on the coordinates in a window
	(only within the background window, if outside of it's bounds - returns Vec(0,0))
	*/
	sf::Vector2i coordChange(const sf::Vector2i& pointPos);
	
	/*
	Get the position of the mouse relative to the texture window
	\param windowSize: size of the game window
	\param mousePosX: X coordinate of the mouse position in the game window
	\param mousePosY: Y coordinate of the mouse position in the game window

	\return mouse position within the texture window, (-1, -1) if outside
	*/
	const sf::Vector2i relativeMousePos(int mousePosX, int mousePosY) const;

public:

	/*
	Get a reference to the texture sprite.
	\return sprite containing the texture to be rendered (with all the parameters set)
	*/
	sf::Sprite& getTexture();
};