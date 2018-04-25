#ifndef CONCAVESHAPE_HPP
#define CONCAVESHAPE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include "vectorutils.hpp"

using namespace std;

class ConcaveShape {
public:
	vector<sf::Vector2f> vertices;
	vector<vector<sf::Vector2f> > relConvexPoints;
	vector<vector<sf::Vector2f> > absConvexPoints;
	vector<sf::ConvexShape*> convexShapes;

	ConcaveShape() {}
	ConcaveShape(vector<sf::Vector2f> points);

	void setFillColor(sf::Color col);
	void setRotation(float angle);
	void setPosition(sf::Vector2f pos);
	float getTop();
	float getLeft();
	float getBottom();
	float getRight();

	~ConcaveShape();

};

#endif