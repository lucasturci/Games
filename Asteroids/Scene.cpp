#include "include/Scene.hpp"
#include <iostream>

using namespace std;
long long int Timer::idCounter;

/////////////// PROTECTED ///////////////
Scene::Scene(int windowWidth, int windowHeight) {

	isRunning = false;
	paused = false;
	sceneFrame = new Frame(windowWidth, windowHeight);
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	Timer::idCounter = 0;
}

void Scene::startTimer(Timer t) {
	t.running = true;
	t.reset();
	timers.push_back(t);
}

void Scene::stopTimer(Timer t) {
	std::vector<Timer>::iterator it;
	for(it = timers.begin(); it < timers.end(); ++it) {
		if((*it) == t) {
			// cout << "ok" << endl;
			timers.erase(it);
			return;
		}
	}
}

void Scene::m_update() {
	int i;
	for(i = 0; i < timers.size(); ++i) {
		if(timers[i].running) {
			if(!paused) timers[i].count();
			else timers[i].restart();
		}
		else {
			timers.erase(timers.begin() + i);
			i--;
		}
	}
	if(!paused) update();
}


void Scene::addEntity(sf::Drawable* entity) {
	sceneFrame->addEntity(entity);
}

void Scene::removeEntity(sf::Drawable* entity) {
	sceneFrame->removeEntity(entity);
}

void Scene::addChild(Frame* child) {
	sceneFrame->addChild(child);
}

void Scene::removeChild(Frame* child) {
	sceneFrame->removeChild(child);
}