#pragma once

#include <raylib.h>

class Sprite
{
public:
	Texture2D texture;
	Rectangle textureRect;
	Vector2 origin = { 0,0 };

	Sprite() {};

	Sprite(Texture2D texture)
		:texture(texture)
	{
		origin.x = texture.width / 2.0f;
		origin.y = texture.height / 2.0f;
	};

	Sprite(const Texture2D& texture, const Rectangle& rect)
		:texture(texture), textureRect(rect)
	{
		origin.x = textureRect.width / 2.0f;
		origin.y = textureRect.height / 2.0f;
	}
};