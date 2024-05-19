#include "Animation.h"
#include <iostream>
Animation::Animation() {};

Animation::Animation(const std::string& name,
					const sf::Texture& t,
					int frameCount,
					int speed,
					const Vec2& offset,
					const Vec2& size)
	: _name(name)
	, _frameCount(frameCount)
	, _speed(speed)
	, _offset(offset)
	, _size(size)
{
	_sprite.setTexture(t);
	_sprite.setOrigin(_size.x / 2.0f, _size.y / 2.0f);
	_sprite.setTextureRect(sf::IntRect(_offset.x, _offset.y, _size.x, _size.y));
}

void Animation::onEnd()
{
	_hasEnded = true;
}

void Animation::update()
{

	_animFrame = (int)(_currentFrame / _speed) % _frameCount;


	int posX = _offset.x + (_animFrame * _size.x);
	int posY = _offset.y;

	_sprite.setTextureRect(sf::IntRect(posX, posY, _size.x, _size.y));

	if (_currentFrame == _frameCount*_speed-1) { 
		onEnd(); }
	_currentFrame++;
}

bool Animation::hasEnded() const
{
	return _hasEnded;
}

const std::string& Animation::getName() const
{
	return _name;
}

const Vec2& Animation::getSize() const
{
	return _size;
}

sf::Sprite& Animation::getSprite()
{
	return _sprite;
}

const int Animation::getInterval() const
{
	return _speed;
}

void Animation::setInterval(int interval)
{
	_speed = interval;
}