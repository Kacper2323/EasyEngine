#include "Assets.h"
#include <iostream>

void Assets::addTexture(const std::string& name, const std::string& path)
{
	Texture2D texture = LoadTexture(path.c_str());

	/*if (!texture.loadFromFile(path))
	{
		std::cerr << "Failed to load texture: " << name << " at " << path << std::endl;
	}*/


	_textures[name] = texture;
}


void Assets::addFont(const std::string& name, const std::string& path)
{
	Font font = LoadFont(path.c_str());

	/*if (!font.loadFromFile(path))
	{
		std::cerr << "Failed to load font: " << name << " at " << path << std::endl;
	}*/

	_fonts[name] = font;
}

void Assets::addAnimation(	const std::string& name,
								const std::string& textureName,
								int frameCount,
								int speed,
								const Vec2& offset,
								const Vec2& size)
{
	
	Animation animation(name, _textures.at(textureName), frameCount, speed, offset, size);

	_animations[name] = animation;
}

std::map<std::string, Texture2D>& Assets::getTextures()
{
	return _textures;
}

Texture2D& Assets::getTexture(const std::string& texture)
{
	return _textures[texture];
}

Animation& Assets::getAnimation(const std::string& animation)
{
	return _animations[animation];
}

Font& Assets::getFont(const std::string& font)
{
	return _fonts[font];
}