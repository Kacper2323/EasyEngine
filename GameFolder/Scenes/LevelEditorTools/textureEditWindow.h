#pragma once

#include <SFML/Graphics.hpp>
#include "../../Utils/messages.h"

class TextureEditWindow
{
	sf::Vertex _selection[4];
	sf::Sprite _sprite;
	sf::Vector2u _position;
	sf::Vector2u _size;
	float backgroundScale = 1;
	float relativeScale = 1;

public:
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

	const sf::Vector2i relativeMousePos(sf::Vector2u windowSize, sf::Vector2i mousePos) const;
	
	sf::Sprite getTexture();
};