#ifndef FRAME_HPP
#define FRAME_HPP

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

class GameManager;

class Frame {
	friend class GameManager; //Game manager has access to private members to draw the rectangles
private:
	bool hasBorder;
	
	sf::RectangleShape borderRect;
	sf::Color borderColor;
	float borderThickness;
	
	sf::RectangleShape backgroundRect;
	sf::Color backgroundColor;
	
	Frame* parent;
	std::vector<Frame*> children;
	std::vector<sf::Drawable*> entities;
	
	sf::Vector2f pos; //relative position
	sf::Vector2f abspos; //absolute position

public:

	float width;
	float height;
	
	Frame();
	Frame(float width, float height, sf::Color = sf::Color::White, float a = 0.0f, sf::Color = sf::Color::Black);
	
	/* Setters*/
	void setPosition(float x, float y);
	void setPosition(sf::Vector2f);
	void setAbsolutePosition(float x, float y);
	void setBackgroundColor(sf::Color);
	void setBorderColor(sf::Color col);
	void setBorderThickness(float thickness);
	
	/* Getters */
	sf::Vector2f getPosition(); //gets the relative position
	
	void addChild(Frame* child);
	void removeChild(Frame* child);
	void addEntity(sf::Drawable* entity);
	void removeEntity(sf::Drawable* entity);
	virtual ~Frame();
};

#endif