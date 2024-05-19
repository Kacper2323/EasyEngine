#pragma once

#include <SFML/Graphics.hpp>

class Shape
{
private:
    float posX = 0;
    float posY = 0;
    float radius = 50;
    float width = 20;
    float height = 20;
    
public:
    sf::CircleShape circle;
    sf::RectangleShape rectangle;
    bool shapeFlag;
    std::string name;
    sf::Text namm;
    float velX = 0;
    float velY = 0;
    float color[3] = { 0, 0, 0 };
    float scale = 1;

    Shape(std::string name, float posX, float posY, float velX, float velY, float radius);
    Shape(std::string name, float posX, float posY, float velX, float velY, float width, float height);

    void Update();
    void checkCollision();
    void formatName(sf::Font& font);
};

//for a circle
Shape::Shape(std::string name, float posX, float posY, float velX, float velY, float radius)
    :circle(radius, 32), shapeFlag(0)
    , name(name), posX(posX), posY(posY), velX(velX), velY(velY), radius(radius)
{};

//for a rectangle
Shape::Shape(std::string name, float posX, float posY, float velX, float velY, float width, float height)
    :rectangle(sf::Vector2f(width, height)), shapeFlag(1)
    , name(name), posX(posX), posY(posY), velX(velX), velY(velY), height(height), width(width)
{};

void Shape::Update()
{
    posX += velX;
    posY += velY;

    if (shapeFlag == 0)
    {
        circle.setPosition(posX, posY);
        circle.setFillColor(sf::Color(color[0] * 255, color[1] * 255, color[2] * 255));
        circle.setScale(scale, scale);
    }
    else
    {
        rectangle.setPosition(posX, posY);
        rectangle.setFillColor(sf::Color(color[0] * 255, color[1] * 255, color[2] * 255));
        rectangle.setScale(scale, scale);
    }
};

void Shape::checkCollision()
{
    sf::FloatRect bounds;

    if (shapeFlag == 0) {
        bounds = circle.getGlobalBounds();
    }
    else {
        bounds = rectangle.getGlobalBounds();
    }

    if (bounds.top <= 0 || (bounds.top + bounds.height) >= 720)
    {
        velY *= -1;
    }
    if (bounds.left <= 0 || (bounds.left + bounds.width) >= 1280)
    {
        velX *= -1;
    }
};

void Shape::formatName(sf::Font& font)
{
    sf::FloatRect bounds;

    if (shapeFlag == 0) {
        bounds = circle.getGlobalBounds();
    }
    else {
        bounds = rectangle.getGlobalBounds();
    }

    sf::Text text(name, font, (int)(bounds.width/4));
    sf::FloatRect textBounds;
    float middlex, middletext;
    float middley, middleteyt;

    middlex = bounds.left + (bounds.width / 2);
    middley = bounds.top + (bounds.height / 2);

    textBounds = text.getLocalBounds();
    middletext = textBounds.width / 2;

    text.setPosition(middlex - middletext, middley - textBounds.height);
    namm = text;
}