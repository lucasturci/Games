#ifndef SCENE_HPP
#define SCENE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Frame.hpp"
#include "Timer.hpp"
#include <vector>

class GameManager; //forward declaration of GameManager to avoid circular dependency

/* Base Abstract class accessable by the Game Manager and that stores the game logic and entities for a given scene*/
class Scene {
	friend class GameManager; //Game Manager has access to Scene's protected members
private:
	
	std::vector<Timer> timers;
	void m_update();
	int m_windowWidth;
	int m_windowHeight;
protected:
	bool isRunning; //important: tells the Game Manager when to stop the game loop
	Frame* sceneFrame; //base Frame for all drawable entities and other frames
	
	/* Constructor */
	Scene(int windowWidth, int windowHeight);
	
	/* Getters */
	int getWindowWidth() {return m_windowWidth;}
	int getWindowHeight() {return m_windowHeight;}
	
	/* Member protected functions */
	void addEntity(sf::Drawable* entity);
	void removeEntity(sf::Drawable* entity);
	void addChild(Frame* entity);
	void removeChild(Frame* entity);
	
	/* Virtual functions */
	virtual void update() =0;
	
	/* Timer */
	void startTimer(Timer t);
	void stopTimer(Timer t);
	
	/* Event Handlers*/
	virtual void onStart() {}
	virtual void onTerminate() {}
	virtual void lostFocus() {}
	virtual void gainedFocus() {}
	virtual void onKeyPressed(sf::Keyboard::Key) {} //called for every frame update of window in which the key is down, not just once. Actually, it is not called for every frame in which the key is down. It works like when you try to type multiple of a letter by holding the key. It types once, then there is a delay, then, if you don't release the key, it keeps calling this function just as you would want. A solution is to make a bool, and when the key is pressed, you trigger it, than when it is released, you deactivate it.
	virtual void onKeyReleased(sf::Keyboard::Key) {}
	virtual void onMouseButtonPressed(sf::Mouse::Button button, int x, int y) {}
	virtual void onMouseButtonReleased(sf::Mouse::Button button, int x, int y) {}
	virtual void onMouseWheelScrolled(float delta, int x, int y) {}
	
	virtual ~Scene() { }//if(sceneFrame) delete sceneFrame; }
	
public:
	bool paused;
	
};

#endif