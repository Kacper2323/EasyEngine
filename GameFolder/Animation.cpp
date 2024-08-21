#include "Animation.h"
#include <iostream>
Animation::Animation() {};

Animation::Animation(const std::string& name,
					const Texture2D& t,
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
	Rectangle rect = { _offset.x, _offset.y, _size.x, _size.y };
	_sprite = Sprite(t, rect);
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

	Rectangle rect = { posX, posY, _size.x, _size.y };

	_sprite.textureRect = rect;

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

Sprite Animation::getSprite()
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

void Animation::flipHorizontal(bool flip)
{
	if (flip)
		horizontalFlip = -1;
	else
		horizontalFlip = 1;
}