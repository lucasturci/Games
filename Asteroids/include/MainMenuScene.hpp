#ifndef MAINMENUSCENE_HPP
#define MAINMENUSCENE_HPP
#include "Definitions.hpp"
#include "Scene.hpp"
#include "Frame.hpp"

using namespace std;

class MainMenuScene: public Scene {
public:
	sf::Text *title;
	sf::Text *space_to_play;
	sf::Font font;
	
	MainMenuScene(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight) {
		sf::FloatRect rect;
		
		sceneFrame->setBackgroundColor(sf::Color::White);
		font.loadFromFile("res/sansation.ttf");
		
		/* Title Text object configuration */
		title = new sf::Text("ASTEROIDS", font);
		title->setFillColor(sf::Color::Black);
		rect = title->getLocalBounds();
		title->setOrigin(rect.left + rect.width/2.0f, rect.top + rect.height/2.0f);
		title->setPosition(windowWidth/2.0f, rect.height/2.0f + 10);
		
		/* "Space To Play" Text object configuration */
		space_to_play = new sf::Text("Pressione Espaco para Jogar", font);
		space_to_play->setFillColor(sf::Color::Black);
		space_to_play->setCharacterSize(22);
		rect = space_to_play->getLocalBounds();
		space_to_play->setOrigin(rect.left + rect.width/2.0f, rect.top + rect.height/2.0f);
		space_to_play->setPosition(windowWidth/2.0f, windowHeight/2.0f + 10);
		
		addEntity(space_to_play);
		addEntity(title);
		
	}
	
	void update() {}
	
	void onKeyPressed(sf::Keyboard::Key k) {
		if(k == sf::Keyboard::Key::Space) {
			isRunning = false;
		}
	}
	
	void stopRunning() {
		
	}

	~MainMenuScene() {
		delete space_to_play;
		delete title;
	}
	
};

#endif