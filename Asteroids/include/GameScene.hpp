#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include <cmath>
#include <vector>
#include <stack>
#include "Timer.hpp"
#include "Definitions.hpp"
#include "Scene.hpp"
#include "Frame.hpp"
#include "vectorutils.hpp"
#include "Boulder.hpp"
#include "Collision.hpp"
#include "Hyperspace.hpp"

using namespace std;

typedef struct ship {
	float force = 1.0f;
	float maxSpeed = 12;
	float rotationSpeed = PI/FRAMERATE;
	float deacceleration = 0.01125f;
	sf::Vector2f pos;
	sf::Vector2f speed = zerovec;
	float angle; //em radianos
	bool rotatingleft;
	bool rotatingright;
	bool accelerating;
	sf::ConvexShape shape;
	sf::ConvexShape fireShape;
	Timer waitToShoot;
	bool invunerable;
	Timer invunerableTimer, waitToRespawn;
	int invunerableTick;
	bool canShoot, canHyperspace, willShoot, transparent, active;
	
	ship();		
	void enableMissile();
	void enableHyperspace();
	void reactivate();

} SHIP;

typedef struct missile {
	sf::Vector2f pos;
	sf::Vector2f speed;
	const float initialSpeed = 10;
	const float radius = 1.5f;
	sf::CircleShape shape;
	bool isGone;
	Timer countdown;
	
	missile();
	void destroy();
} missile;

typedef struct explosion {
	float transparency;
	float scale;
	sf::Sprite sprite;
	
	explosion();
	explosion(sf::Texture tex);
} explosion;


class GameScene: public Scene {
public:
	sf::Font HUDFont;
	int points, highscore;
	sf::Text pointsText;
	sf::Text pausedText;
	sf::Text playAgainText;
	sf::Text finalHighscoreText;
	sf::Text finalScoreText;
	sf::Text highscoreText;
	int lives;
	SHIP ship;
	SHIP shipAnimation;
	stack<sf::Sprite*> lifeIcons;
	sf::Texture explosionTexture;
	sf::Texture lifeIconTexture;
	int totalAsteroids;
	int level;
	bool inHyperspace;
	bool betweenLevels;
	bool gameOver;
	Timer waitToChangeLevel;
	Asteroid * placeholder;

	vector<missile*> missiles;
	vector<Asteroid*> boulders;
	vector<explosion*> explosions;

	/* HUD */
	
	GameScene();
	GameScene(int windowWidth, int windowHeight);
	
	/* Events */
	void onKeyPressed(sf::Keyboard::Key key);
	void onKeyReleased(sf::Keyboard::Key key);
	
	void addBoulderEntity(Asteroid * b);
	void onStart();
	void shipUpdate();
	void spawnAsteroids();
	void hyperspace();
	void setupLevel();
	void setupGame();
	missile *createMissile();
	void missilesUpdate();
	void bouldersUpdate();
	void collisionChecking();
	void explosionsUpdate();
	void animationUpdate();
	void HUDUpdate();
	void respawn();
	void countInvunerable();
	void breakAsteroid(Asteroid *, sf::Vector2f);
	
	void update();
	
	~GameScene();
};

#endif