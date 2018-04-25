#ifndef BOULDER_HPP
#define BOULDER_HPP

#include "ConcaveShape.hpp"
#include "vectorutils.hpp"
#include "Definitions.hpp"
#include <SFML/System.hpp>

using namespace std;

class Asteroid {
public:	
	ConcaveShape shape;
	
	float rotationSpeed;
	float angle;
	sf::Vector2f position;
	sf::Vector2f speed;
	
	Asteroid * yGhost;
	Asteroid * xGhost;
	bool isGone = false;

	int type;
	char size;

	Asteroid() {
		angle = 0;
		position = sf::Vector2f(0.0f, 0.0f);
		rotationSpeed = (rand() / (RAND_MAX*1.0f))*0.1f - 0.05f; //randomico
		speed = sf::Vector2f((rand() / (RAND_MAX*1.0f))*4.0f - 2.0f, (rand() / (RAND_MAX*1.0f))*4.0f - 2.0f); //randomico
		xGhost = NULL;
		yGhost = NULL;
	}
	void createShape(vector<sf::Vector2f> vertices);
	void setPosition(sf::Vector2f pos);
	void setPosition(float x, float y);
	void setRotation(float angle); //angulo em radianos
	void destroy();

	virtual Asteroid * copy() =0;
	virtual ~Asteroid() {
		if(yGhost) yGhost->yGhost = NULL;
		if(xGhost) xGhost->xGhost = NULL;
	}

};


class BigBoulder: public Asteroid {
public:
	BigBoulder(int type = 1);
	Asteroid * copy();
};

class MediumBoulder: public Asteroid {
public:
	MediumBoulder(int type = 1);
	Asteroid * copy();
};

class SmallBoulder: public Asteroid {
public:
	SmallBoulder(int type = 1);
	Asteroid * copy();
};


#endif