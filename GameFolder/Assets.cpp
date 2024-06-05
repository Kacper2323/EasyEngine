#include "Assets.h"
#include <iostream>

void Assets::addTexture(const std::string& name, const std::string& path)
{
	sf::Texture texture;

	if (!texture.loadFromFile(path))
	{
		std::cerr << "Failed to load texture: " << name << " at " << path << std::endl;
	}

	_textures[name] = texture;
}

void Assets::addSound(const std::string& name, const std::string& path)
{
	sf::SoundBuffer soundBuffer;

	if (!soundBuffer.loadFromFile(path))
	{
		std::cerr << "Failed to load sound: " << name << " at " << path << std::endl;
	}

	Sound sound(soundBuffer);

	_sounds[name] = sound;
}

void Assets::addFont(const std::string& name, const std::string& path)
{
	sf::Font font;

	if (!font.loadFromFile(path))
	{
		std::cerr << "Failed to load font: " << name << " at " << path << std::endl;
	}

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

std::map<std::string, sf::Texture>& Assets::getTextures()
{
	return _textures;
}

sf::Texture& Assets::getTexture(const std::string& texture)
{
	return _textures[texture];
}

Animation& Assets::getAnimation(const std::string& animation)
{
	return _animations[animation];
}

Sound& Assets::getSound(const std::string& sound)
{
	return _sounds[sound];
}

sf::Font& Assets::getFont(const std::string& font)
{
	return _fonts[font];
}