#pragma once

#include "Animation.h"
#include <SFML/Audio.hpp>

/*
Class for storing a sound from an sf::SoundBuffer class.
*/
class Sound
{
	sf::SoundBuffer _soundBuffer;

public:
	sf::Sound		sound;

	Sound() {};
	Sound(const sf::SoundBuffer soundBuffer)
		:_soundBuffer(soundBuffer)
	{
		sound.setBuffer(_soundBuffer);
	}
};


/*
Class for storing game assets. Contains sf::Texture, Animation, Sound and sf::Font.
*/
class Assets
{
	std::map<std::string, sf::Texture> _textures;
	std::map<std::string, Animation> _animations;
	std::map<std::string, Sound> _sounds;
	std::map<std::string, sf::Font> _fonts;

public:

	/*
	Add a texture to game assets.
	\param name: name of the texture
	\param path: path to the image (.png or .jpeg)
	*/
	void addTexture(const std::string& name, const std::string& path);

	/*
	Add a sound to game assets.
	\param name: name of the sound
	\param path: path to the sound file (e.g. .mp3)
	*/
	void addSound(const std::string& name, const std::string& path);

	/*
	Add a font to game assets.
	\param name: name of the font
	\param path: path to the font file (.ttf)
	*/
	void addFont(const std::string& name, const std::string& path);

	/*
	Add an animation to game assets.
	\param name: name of the animation
	\param textureName: name of the texture to be used (must be loaded to game assets)
	\param frameCount: number of frames in the animation
	\param speed: number of frames in game between frames of animation (the lower, the faster the animation will play)
	\param offset: the position of the animation within the texture
	\param size: size of the animation rectangle within the texture
	*/
	void addAnimation(	const std::string& name, 
						const std::string& textureName,
						int frameCount,
						int speed,
						const Vec2& offset,
						const Vec2& size); 

	/*
	Get a texture with a given name.
	\param texture: name of the texture
	\return reference to the texture stored in the assets object
	*/
	sf::Texture& getTexture(const std::string& texture);

	/*
	Get a map of all textures with their names as keys and references as values.
	\return [name: texture] map where 'texture' is a reference to a texture object stored in the assets object
	*/
	std::map<std::string, sf::Texture>& getTextures();

	/*
	Get an animation with a given name.
	\param animation: name of the animation
	\return reference to an animation stored in assets
	*/
	Animation& getAnimation(const std::string& animation);

	/*
	Get a sound with a given name.
	\param sound: name of the sound
	\return reference to a sound stored in assets
	*/
	Sound& getSound(const std::string& sound);

	/*
	Get a font with a given name.
	\param font: name of the font
	\return reference to a font stored in assets
	*/
	sf::Font& getFont(const std::string& font);
};