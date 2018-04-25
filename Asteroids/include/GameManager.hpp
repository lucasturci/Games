#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Definitions.hpp"
#include "Scene.hpp"
#include "Frame.hpp"

class Transition {
public:
	enum TransitionType{
		None,
		Fade
	};
};

class GameManager {
private:
	bool running;
	int windowWidth;
	int windowHeight;
	sf::RenderWindow window;
	Scene* scene;
	
	void drawFrame(Frame*, sf::Vector2f);
	void draw();
	void gameLoop();
	void handleInput();
public:

	/* Constructors */
	GameManager();

	/* User methods */
	void setWindow(int w, int h);
	void runScene(Scene*, Transition::TransitionType a = Transition::TransitionType::None);
	void finishGame();
	
	/* Destructor */
	~GameManager() {
		
	}
};

#endif 
