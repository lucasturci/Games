#include "include/Frame.hpp"
#include <stdio.h>

//////////////// PUBLIC CONSTRUCTORS ////////////////
Frame::Frame():
	backgroundColor(sf::Color::White),
	borderColor(sf::Color::Black)
{}

Frame::Frame(float width, float height, sf::Color backgcolor, float thickness, sf::Color bordColor) {
	
	Frame::width = width;
	Frame::height = height;
	backgroundColor = backgcolor;
	borderThickness = thickness;
	borderColor = bordColor;
	
	hasBorder = (thickness > 0.0f);
	if(hasBorder) {
		borderRect = sf::RectangleShape(sf::Vector2f(width, height));
		borderRect.setPosition(0.0f, 0.0f);
		borderRect.setFillColor(bordColor);
	}
	
	backgroundRect = sf::RectangleShape(sf::Vector2f(width - 2.0f*thickness, height - 2.0f*thickness));
	backgroundRect.setFillColor(backgcolor);
}

/* Setters */
void Frame::setPosition(float x, float y) {
	float xp = parent? parent->abspos.x : 0.0f;
	float yp = parent? parent->abspos.y : 0.0f;

	pos.x = x;
	pos.y = y;
	abspos.x = xp + pos.x;
	abspos.y = yp + pos.y;
	
	borderRect.setPosition(abspos);
	backgroundRect.setPosition(abspos + sf::Vector2f(borderThickness, borderThickness));
}

void Frame::setPosition(sf::Vector2f p) {
	setPosition(p.x, p.y);
}

void Frame::setAbsolutePosition(float x, float y) {
	float xp = parent? parent->abspos.x : 0.0f;
	float yp = parent? parent->abspos.y : 0.0f;
	
	abspos.x = x;
	abspos.y = y;
	pos.x = x - xp;
	pos.y = y - yp;
}

void Frame::setBackgroundColor(sf::Color col) {
	backgroundColor = col;
	backgroundRect.setFillColor(col);
}

void Frame::setBorderColor(sf::Color col) {
	borderColor = col;
	borderRect.setFillColor(col);
}

void Frame::setBorderThickness(float thickness) {
	borderThickness = thickness;
	backgroundRect = sf::RectangleShape(sf::Vector2f(width - 2*thickness, height - 2*thickness));
	backgroundRect.setFillColor(backgroundColor);
	backgroundRect.setPosition(abspos + sf::Vector2f(thickness, thickness));
}

/* Getters */
sf::Vector2f Frame::getPosition() {
	return pos;
}

/* Public member functions*/
void Frame::addChild(Frame* child) {
	children.push_back(child);
	child->parent = this;
	child->setPosition(child->pos.x, child->pos.y);
}

void Frame::addEntity(sf::Drawable* entity) {
	entities.push_back(entity);
}

void Frame::removeChild(Frame* child) {
	std::vector<Frame*>::iterator it;
	
	for(it = children.begin(); it < children.end();) {
		if(*it == child) {
			children.erase(it);
			break;
		}
		else ++it;
	}
	
}

void Frame::removeEntity(sf::Drawable* entity) {
	std::vector<sf::Drawable*>::iterator it;
	
	for(it = entities.begin(); it != entities.end();) {
	
		if(*it == entity) {
			entities.erase(it);
			break;
		}
		else ++it;
	}
	
}

Frame::~Frame() {

}