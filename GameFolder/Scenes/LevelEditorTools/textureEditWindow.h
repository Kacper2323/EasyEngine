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

class TextureEditWindow
{
	sf::Vertex _selection[4];
	sf::Sprite _sprite;
	sf::Vector2u _position;	//position of the background
	sf::Vector2u _size;	//size of the texture
	float backgroundScale = 1;	//scale of the background
	float relativeScale = 1;	//scale of the texture relative to background

	sf::Vector2i mouseCoords;
	sf::Vector2i mouseCoordsInTexture;
	sf::IntRect textureRect;

	//temp test
	int rectWidth = 0;
	int rectHeight = 0;
	
public:
	mouseState mouseState = mS_DEFAULT;
	sf::RectangleShape selection_;
	sf::RectangleShape background;
	TextureEditWindow()
	{};


	/*
	Set the texture to be displaye.
	\param texture: referance to sf::Texture to be displayed
	*/
	void useTexture(sf::Texture& texture);


	/*
	Set the background for the texture view window.
	*/
	void setBackground();

	/*
	Set the background scale, use this first before other functions.
	\param windowSize: size of the rendering window
	*/
	void scaleToMainWindow(sf::Vector2u windowSize);


	/*
	Zoom the texture in and out within the background.
	\param ticks: ticks of mouse wheel (positive if up, negative if down)
	\param mousePos: X and Y coordinates (within the rendering window) of the mouse at the moment of scroll
	*/
	void zoom(int ticks, int mousePosX, int mouosPosY);

	/*
	Pan the texture within the background.
	\param mousePosX: X coordinate of the mouse position
	\param mousePosY: Y coordinate of the mouse position
	*/
	void pan(int mousePosX, int mousePosY);

	/*
	Set panning. Causes the texture to move with the mouse.
	\param mousePosX: X coordinate of the mouse position
	\param mousePosY: Y coordinate of the mouse position
	*/
	void setPanning(sf::Vector2i coordsInTexture);

	/*
	Stop panning. Causes the texture to stop moving with the mouse.
	*/
	void stopPanning();

	/*
	Start a selection rectangle on the texture window.
	\param mousePos: mouse position in the game window
	*/
	void startSelection(sf::Vector2i mousePos);

	/*
	Stop the selection.
	\return selection coordinates (left, top, width, height)
	*/
	sf::IntRect stopSelection();

	/*
	Selection system. Resizes the selection to the position of the mouse.
	\param mousePosX: X coordinate of the mouse position
	\param mousePosY: Y coordinate of the mouse position
	*/
	void selection(int mousePosX, int mousePosY);

	/*
	Calculates the coordinates of a point on a texture based on the coordinates in a window
	(only within the background window, if outside of it's bounds - returns Vec(0,0))
	*/
	sf::Vector2i coordChange(sf::Vector2i pointPos);

	const sf::Vector2i relativeMousePos(sf::Vector2u windowSize, int mousePosX, int mousePosY) const;
	
	sf::Sprite& getTexture();
};