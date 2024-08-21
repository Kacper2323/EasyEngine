#pragma once

#include <map>
#include <string>
#include <raylib.h>

#include "Animation.h"

/*
Class for storing game assets. Contains sf::Texture, Animation, Sound and sf::Font.
*/
class Assets
{
	std::map<std::string, Texture2D> _textures;
	std::map<std::string, Animation> _animations;
	std::map<std::string, Font> _fonts;

public:

	/*
	Add a texture to game assets.
	\param name: name of the texture
	\param path: path to the image (.png or .jpeg)
	*/
	void addTexture(const std::string& name, const std::string& path);

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
	Texture2D& getTexture(const std::string& texture);

	/*
	Get a map of all textures with their names as keys and references as values.
	\return [name: texture] map where 'texture' is a reference to a texture object stored in the assets object
	*/
	std::map<std::string, Texture2D>& getTextures();

	/*
	Get an animation with a given name.
	\param animation: name of the animation
	\return reference to an animation stored in assets
	*/
	Animation& getAnimation(const std::string& animation);

	/*
	Get a font with a given name.
	\param font: name of the font
	\return reference to a font stored in assets
	*/
	Font& getFont(const std::string& font);
};