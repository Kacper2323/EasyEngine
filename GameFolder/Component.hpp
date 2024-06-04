#pragma once

#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include "Animation.h"

/*
Component class. New components are made by inheriting this one.
Contains a 'has' property which is set to true when a component is added to an entity.

//ADD COPY CONSTRUCTORS!!!
*/
class Component
{
public:
	bool has = false;
	Component()
	{}
};

class CGravity : public Component
{
public:
	float gravAcc = 0.0f;
	CGravity() {};
	CGravity(float acc)
		:gravAcc(acc)
	{};
};

class CSprite : public Component
{
public:
	sf::Sprite sprite;
	CSprite() {};
	CSprite(sf::Sprite& spritee)
		:sprite(spritee)
	{
		sf::IntRect rect = spritee.getTextureRect();
		sprite.setOrigin(rect.width / 2.0f, rect.height / 2.0f);
	};
	CSprite(sf::Texture& t, const sf::IntRect& rect)
	{
		sprite.setTexture(t);
		sprite.setOrigin(rect.width / 2.0f, rect.height / 2.0f);
		sprite.setTextureRect(rect);
	}
};

class CAnimation : public Component
{
public:
	Animation animation;

	CAnimation() {};
	CAnimation(Animation& anim)
		:animation(anim)
	{};
};

class CTransform : public Component
{
public:
	Vec2 pos = { 0.0f, 0.0f };
	Vec2 prevPos = { 0.0f, 0.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	Vec2 acceleration = { 0.0f, 0.0f };
	float angle = 0;

	CTransform() {};
	CTransform(const Vec2& posin, const Vec2& velocityin, float anglein)
		:pos(posin),velocity(velocityin), angle(anglein)
	{}
};


class CBoundingBox : public Component
{
public:
	Vec2 size = { 0.0f, 0.0f };
	Vec2 halfSize = { 0.0f, 0.0f };
	bool physical = true;

	CBoundingBox() {};
	CBoundingBox(const Vec2& size)
		:size(size)
	{
		halfSize = Vec2(size.x/2, size.y/2);
	}
};

class CCollision : public Component
{
public:
	CCollision() {};
};

class CScore : public Component
{
public:
	int score = 0;

	CScore() {};
	CScore(int s)
		:score(s)
	{};
};

class CLifespan : public Component
{
public:
	int remaining = 0;
	int total = 0;

	CLifespan() {};
	CLifespan(int total)
		:total(total), remaining(total)
	{};
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool leftClick = false;
	bool rightClick = false;
	bool space = false;
	bool LShift = false;

	CInput() {};
};
