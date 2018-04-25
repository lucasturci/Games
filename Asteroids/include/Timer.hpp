#ifndef TIMER_HPP
#define TIMER_HPP

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Function.hpp"

class Scene;

class Timer {
	friend class Scene;
private:
	sf::Clock clock;
	sf::Time initialTime;
	sf::Time time;
	Function trigger;
	bool running;
	bool repeat;
	
	void count() {
		sf::Time t = clock.restart();
		if(!paused) time -= t;
		
		if(time < sf::Time::Zero) {
			trigger();
			if(repeat) reset();
			else running = false;
		}
	}

public:
	bool paused;
	long long int id;
	static long long int idCounter;

	Timer(): paused(false), repeat(false) {}
	
	Timer(sf::Time t, Function f, bool repeat = false): paused(false) {
		initialTime = time = t;
		trigger = f;
		id = idCounter;
		idCounter++;
		this->repeat = repeat;
	}
	
	void create(sf::Time t, Function f) {
		*this = Timer(t, f);
	}

	void restart() { clock.restart(); }
	void reset() {
		restart();
		time = initialTime;
	}

	
	bool operator== (Timer t) {
		return id == t.id;
	}
	
};

#endif