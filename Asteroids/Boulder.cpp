#include "include/Boulder.hpp"
#include <iostream>
#include <cmath>

using namespace std;

void Asteroid::createShape(vector<sf::Vector2f> vertices) {
	position = sf::Vector2f(0.0f, 0.0f);

	shape = ConcaveShape(vertices);
	shape.setFillColor(sf::Color(180, 180, 180));
	shape.setPosition(position);
}

void Asteroid::setPosition(sf::Vector2f pos) {
	shape.setPosition(pos);
	position = pos;
}
void Asteroid::setPosition(float x, float y) { setPosition(sf::Vector2f(x, y)); }

void Asteroid::setRotation(float angle) {
	this->angle = angle;
	shape.setRotation(degrees(angle));
}

void Asteroid::destroy() {
	if(!isGone) {
		isGone = true;
		if(xGhost) xGhost->destroy();
		xGhost = NULL;
		if(yGhost) yGhost->destroy();
		yGhost = NULL;
	}
}

SmallBoulder::SmallBoulder(int type): Asteroid() {
	SmallBoulder::type = type;
	size = 'S';

	if(type == 1) {
		vector<sf::Vector2f> points;
		
		points.push_back(sf::Vector2f(20.0f, 7.0f)*(8.0f/20.0f));	
		points.push_back(sf::Vector2f(12.0f, 17.0f)*(8.0f/20.0f));
		points.push_back(sf::Vector2f(-1.0f, 15.0f)*(8.0f/20.0f));
		points.push_back(sf::Vector2f(-11.0f, 20.0f)*(8.0f/20.0f));
		points.push_back(sf::Vector2f(-19.0f, 1.0f)*(8.0f/20.0f));
		points.push_back(sf::Vector2f(-11.0f, -6.0f)*(8.0f/20.0f));
		points.push_back(sf::Vector2f(-9.0f, -17.0f)*(8.0f/20.0f));
		points.push_back(sf::Vector2f(12.0f, -18.0f)*(8.0f/20.0f));
		points.push_back(sf::Vector2f(17.0f, -10.0f)*(8.0f/20.0f));

		createShape(points);
	}
}

Asteroid * SmallBoulder::copy() {
	SmallBoulder * boulder = new SmallBoulder(type);
	boulder->rotationSpeed = rotationSpeed;
	boulder->angle = angle;
	boulder->position = position;
	boulder->speed = speed;
	boulder->yGhost = yGhost;
	boulder->xGhost = xGhost;
	boulder->isGone = isGone;
	boulder->type = type;
	boulder->size = size;
	return boulder;
}

MediumBoulder::MediumBoulder(int type): Asteroid() {
	MediumBoulder::type = type;
	size = 'M';
	
	if(type == 1) {
		vector<sf::Vector2f> points;
		
		points.push_back(sf::Vector2f(20.0f, 7.0f));	
		points.push_back(sf::Vector2f(12.0f, 17.0f));
		points.push_back(sf::Vector2f(-1.0f, 15.0f));
		points.push_back(sf::Vector2f(-11.0f, 20.0f));
		points.push_back(sf::Vector2f(-19.0f, 1.0f));
		points.push_back(sf::Vector2f(-11.0f, -6.0f));
		points.push_back(sf::Vector2f(-9.0f, -17.0f));
		points.push_back(sf::Vector2f(12.0f, -18.0f));
		points.push_back(sf::Vector2f(17.0f, -10.0f));

		createShape(points);
	}

}

Asteroid * MediumBoulder::copy() {
	MediumBoulder * boulder = new MediumBoulder(type);
	boulder->rotationSpeed = rotationSpeed;
	boulder->angle = angle;
	boulder->position = position;
	boulder->speed = speed;
	boulder->yGhost = yGhost;
	boulder->xGhost = xGhost;
	boulder->isGone = isGone;
	boulder->type = type;
	boulder->size = size;
	return boulder;
}

BigBoulder::BigBoulder(int type): Asteroid() {
	BigBoulder::type = type;
	size = 'B';

	if(type == 1) {
		vector<sf::Vector2f> points;
		
		points.push_back(sf::Vector2f(20.0f, 7.0f)*(55.0f/20.0f));	
		points.push_back(sf::Vector2f(12.0f, 17.0f)*(55.0f/20.0f));
		points.push_back(sf::Vector2f(-1.0f, 15.0f)*(55.0f/20.0f));
		points.push_back(sf::Vector2f(-11.0f, 20.0f)*(55.0f/20.0f));
		points.push_back(sf::Vector2f(-19.0f, 1.0f)*(55.0f/20.0f));
		points.push_back(sf::Vector2f(-11.0f, -6.0f)*(55.0f/20.0f));
		points.push_back(sf::Vector2f(-9.0f, -17.0f)*(55.0f/20.0f));
		points.push_back(sf::Vector2f(12.0f, -18.0f)*(55.0f/20.0f));
		points.push_back(sf::Vector2f(17.0f, -10.0f)*(55.0f/20.0f));

		createShape(points);
	}
}
	
Asteroid * BigBoulder::copy() {
	BigBoulder * boulder = new BigBoulder(type);
	boulder->rotationSpeed = rotationSpeed;
	boulder->angle = angle;
	boulder->position = position;
	boulder->speed = speed;
	boulder->yGhost = yGhost;
	boulder->xGhost = xGhost;
	boulder->isGone = isGone;
	boulder->type = type;
	boulder->size = size;
	return boulder;
}

