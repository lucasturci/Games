#include "include/GameManager.hpp"
#include <iostream>
#include <time.h>
#include <cstdlib>
///////////// PUBLIC /////////////

using namespace std;

/* Default constructor */
GameManager::GameManager(): window() {
	running = false;
	scene = NULL;
	srand(time(0));
}

/* Creates a window of passed dimensions, framerate still not cohesive*/
void GameManager::setWindow(int w, int h) {
	windowWidth = w;
	windowHeight = h;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 32;
	window.create(sf::VideoMode(w, h), APPLICATION_NAME, sf::Style::Default, settings);
	window.setFramerateLimit(FRAMERATE);
}

/* Runs the scene, i.e. starts the gameloop of this scene*/
void GameManager::runScene(Scene* scene, Transition::TransitionType transition) {
	this->scene = scene;
	scene->isRunning = true;
	running = true;
	
	scene->onStart();
	
	// possible transitions //
	if(transition != Transition::TransitionType::None) {
		if(transition == Transition::TransitionType::Fade) {
			//make animation here and set timer to trigger gameloop
		}
	}
	else gameLoop(); //starts game loop
}

void GameManager::finishGame() {
	
}

//////////// PRIVATE //////////////

/* Handling of the input. Polls all the events that are stored in the window queue of events and acts accordingly*/
void GameManager::handleInput() {
	sf::Event evt;
	while(window.pollEvent(evt)) { //this member function gets the next event and returns false if there isn't one
		if(evt.type == sf::Event::EventType::Closed) {
			window.close();
			running = false;
		}
		else if(evt.type == sf::Event::EventType::KeyPressed) {
			scene->onKeyPressed(evt.key.code);
		}
		else if(evt.type == sf::Event::EventType::KeyReleased) {
			scene->onKeyReleased(evt.key.code);
		}
		else if(evt.type == sf::Event::EventType::LostFocus) {
			scene->lostFocus();
		}
		else if(evt.type == sf::Event::EventType::GainedFocus) {
			scene->gainedFocus();
		}
		else if(evt.type == sf::Event::EventType::MouseButtonPressed) {
			scene->onMouseButtonPressed(evt.mouseButton.button, evt.mouseButton.x, evt.mouseButton.y);
		}
		else if(evt.type == sf::Event::EventType::MouseButtonReleased) {
			scene->onMouseButtonReleased(evt.mouseButton.button, evt.mouseButton.x, evt.mouseButton.y);
		}
	}
}

/* Recursive function that draws a frame, it's child entities and child frames*/
void GameManager::drawFrame(Frame* frame, sf::Vector2f currOrigin) {
	int i;
	
	/* Frame background and border */
	if(frame->hasBorder)
		window.draw(frame->borderRect);
	window.draw(frame->backgroundRect);

	currOrigin += frame->pos; //update relative origin
	
	/* Man, that's awesome!! This piece of code moves what I've drawn so far in the bitmap relatively to the window. (with the setCenter function). What I do is I put the frame I'm drawing the entities onto in the top-left corner of the screen. Then, it's child entities can always be drawn - by it's relative position - on top of the frame. Then I move the bitmap back to it's original position. This might not be the best algorithm (depends on the implementation of the setCenter function). If it results in copying all the bitmap twice, it is definitely not a good one. */
	sf::View v(sf::FloatRect(0, 0, windowWidth, windowHeight));
	v.setCenter(-currOrigin.x + windowWidth/2, -currOrigin.y + windowHeight/2);
	window.setView(v);
	
	for(i = 0; i < frame->entities.size(); ++i) {//child entities
		window.draw(*(frame->entities[i]));
	}
	
	v.setCenter(windowWidth/2, windowHeight/2);
	window.setView(v);

	for(i = 0; i < frame->children.size(); ++i) {
		drawFrame(frame->children[i], currOrigin); //child frames
	}


}

/* Draws the scene by drawing recursively it's root frame */
void GameManager::draw() {
	drawFrame(scene->sceneFrame, sf::Vector2f(0.0f, 0.0f));
}

/* Game loop takes care of input, logic of the scene and rendering */
void GameManager::gameLoop() {
	int i;

	while(scene->isRunning && window.isOpen()) {
		
		/* Handle Input */
		handleInput();
		if(!running) { //user quit the application
			scene->onTerminate();
			break;
		}
		
		/* Logic */
		scene->m_update(); //scene runs its logic
		
		/* Drawing */
		window.clear(sf::Color::Black);
		draw();
		
		window.display(); //flips the buffer, displays what was drawn in the bitmap
	}
}