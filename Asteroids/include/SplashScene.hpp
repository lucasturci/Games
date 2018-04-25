#ifndef SPLASHSCENE_HPP
#define SPLASHSCENE_HPP
#include "Scene.hpp"
#include "Definitions.hpp"
#include <SFML/System.hpp>
#include "Frame.hpp"
#include <iostream>

using namespace std;

class SplashScene: public Scene {
	int counter;
public:
	sf::Text *title;
	sf::Font font;
	
	SplashScene(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight) {
		sf::FloatRect rect;
		counter = 0;
		
		sceneFrame->setBackgroundColor(sf::Color::Black);
		font.loadFromFile("res/sansation.ttf");
		
		/* Title Text object configuration */
		title = new sf::Text(APPLICATION_NAME, font);
		rect = title->getLocalBounds();
		title->setOrigin(rect.left + rect.width/2.0f, rect.top + rect.height/2.0f);
		title->setPosition(windowWidth/2.0f, windowHeight/2.0f);
		
		addEntity(title);
	}
	
	void update() {
		if(counter == 30) { //3 segundos
			isRunning = false;
		}
		counter++;
	}
	
	~SplashScene()  {
		delete title;	
	}
	
};

#endif