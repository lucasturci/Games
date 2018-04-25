#include "include/GameScene.hpp"
#include <cstdlib>
#include <cstdio>
#include <iostream>


/*
	Colisoes:
	Eu tenho que checar colisões com:
	- para cada missel, com o asteroide na grid do missil - SAT
	- para a nave, com asteroide na grid da nave e com o missil na nave inimiga - SAT
	- asteroide com a nave inimiga - N Sei
	- asteroide com o missil da nave inimiga na grid da nave - SAT

	Portanto, eu tenho que guardar numa grid:
	- asteroides e misseis da nave inimiga

	Complexidade:
	O(n) misseis + O(1) (nave com asteroide) + O(1) (nave com nave inimiga) + O(1) (nave com missel da nave inimiga)
	BLZ!!

*/

using namespace std;

ship::ship() {
	angle = 3*PI/2.0f;
	shape = sf::ConvexShape(3);
	shape.setPoint(0, sf::Vector2f(13.0f, 0.0f));
	shape.setPoint(1, sf::Vector2f(-7.0f, 6.0f));
	shape.setPoint(2, sf::Vector2f(-7.0f, - 6.0f));
	shape.setPosition(pos);
	shape.setRotation(degrees(angle));
	shape.setFillColor(sf::Color::White);

	fireShape = sf::ConvexShape(3);
	fireShape.setPoint(0, sf::Vector2f(-12.0f, 0.0f));
	fireShape.setPoint(1, sf::Vector2f(-7.0f, 4.0f));
	fireShape.setPoint(2, sf::Vector2f(-7.0f, -4.0f));
	fireShape.setFillColor(sf::Color::Red);
	fireShape.setPosition(pos);
	fireShape.setRotation(degrees(angle));

	active = true;
	rotatingleft = rotatingright = accelerating = willShoot = false;
	canHyperspace = canShoot = true;
	transparent = invunerable = false;
	invunerableTick = 0;
}

void ship::enableMissile() {
	canShoot = true;
}

void ship::enableHyperspace() {
	canHyperspace = true;
}

void ship::reactivate() {
	active = true;
}

missile::missile() {
	shape.setRadius(this->radius);
	shape.setOrigin(this->radius, this->radius);
	shape.setFillColor(sf::Color::White);
	isGone = false;	
}

void missile::destroy() {
	isGone = true;
}

explosion::explosion() {
	transparency = 1.0;
	scale = 1.0;
}

explosion::explosion(sf::Texture tex) {
	transparency = 1.0;
	scale = 1.0;
}

GameScene::GameScene() : Scene(0, 0) {}
GameScene::GameScene(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight) {
	sf::FloatRect rect;

	highscore = 0; //tirar isso aqui depois, eu vou ter que ler do arquivo, mas esse arquivo vai estar em outro negocio

	sceneFrame->setBackgroundColor(sf::Color::Black);

	HUDFont.loadFromFile("res/sansation.ttf");
	pointsText = sf::Text("0", HUDFont, 24);
	rect = pointsText.getLocalBounds();

	pointsText.setOrigin(rect.width/2.0f, 0.0f);
	pointsText.setPosition(windowWidth/2.0f, 5.0f);

	addEntity(&pointsText);

	char highscoreString[1000];
	sprintf(highscoreString, "%d", highscore);
	highscoreText = sf::Text(highscoreString, HUDFont, 24);
	highscoreText.setFillColor(sf::Color::Blue);
	highscoreText.setOrigin(0.0f, 0.0f);

	highscoreText.setPosition(15.0f, 0.0f);

	addEntity(&highscoreText);

	pausedText = sf::Text("PAUSED", HUDFont, 50);
	rect = pausedText.getLocalBounds();
	
	pausedText.setOrigin(rect.width/2.0f, rect.height/2.0f);
	pausedText.setPosition(windowWidth/2.0f, windowHeight/2.0f);

	playAgainText = sf::Text("PRESS SPACE TO PLAY AGAIN", HUDFont, 25);
	rect = playAgainText.getLocalBounds();
	
	playAgainText.setOrigin(rect.width/2.0f, 0);
	playAgainText.setPosition(windowWidth/2.0f, windowHeight/2.0f + 3);

	finalScoreText = sf::Text("", HUDFont, 35);
	rect = finalScoreText.getLocalBounds();
	
	finalScoreText.setOrigin(rect.width/2.0f, rect.height);
	finalScoreText.setPosition(windowWidth/2.0f, windowHeight/2.0f - 15);

	finalHighscoreText = sf::Text("", HUDFont, 25);
	finalHighscoreText.setFillColor(sf::Color::Blue);
	rect = finalHighscoreText.getLocalBounds();
	
	finalHighscoreText.setOrigin(rect.width/2.0f, 0);
	finalHighscoreText.setPosition(windowWidth/2.0f, windowHeight/2.0f + 53);
	
	explosionTexture.loadFromFile("res/explosion.png");
	lifeIconTexture.loadFromFile("res/life_icon.png");

	waitToChangeLevel = Timer(sf::milliseconds(1500), Function(this, &GameScene::setupLevel));

	placeholder = new BigBoulder();
}

void GameScene::spawnAsteroids() {
	int i, j;
	sf::FloatRect rect = ship.shape.getLocalBounds();
	bool coliding;

	totalAsteroids = min(level + 2, 12);

	for(i = 0; i < totalAsteroids; ++i) {
		boulders.push_back(new BigBoulder());
		do {
			coliding = false;
			boulders[i]->setPosition(sf::Vector2f(rand()%getWindowWidth(), rand()%getWindowHeight()));

			if(	boulders[i]->shape.getRight() > ship.pos.x + rect.left - 30.0f and boulders[i]->shape.getBottom() > ship.pos.y + rect.top - 30.0f and 
				boulders[i]->shape.getTop() < ship.pos.y + rect.top + rect.height + 30.0f and boulders[i]->shape.getLeft() < ship.pos.x + rect.left + rect.width + 30.0f) 
				coliding = true;

		} while(coliding);
		addBoulderEntity(boulders[i]);
	}
}

void GameScene::setupLevel() {
	if(!level) {
		ship.pos.x = getWindowWidth()/2.0;
		ship.pos.y = getWindowHeight()/2.0;
	}
	spawnAsteroids();
	betweenLevels = false;
}

void GameScene::setupGame() {
	level = 0;
	lives = 3;
	points = 0;
	gameOver = betweenLevels = inHyperspace = false;

	removeEntity(&finalScoreText);
	removeEntity(&finalHighscoreText);
	removeEntity(&playAgainText);
	removeEntity(&ship.shape);
	removeEntity(&ship.fireShape);
	ship = SHIP();

	ship.waitToShoot = Timer(sf::milliseconds(300), Function(&ship, &ship::enableMissile));	
	ship.invunerableTimer = Timer(sf::milliseconds(250), Function(this, &GameScene::countInvunerable), true);
	ship.waitToRespawn = Timer(sf::seconds(1), Function(this, &GameScene::respawn));
	addEntity(&ship.fireShape);
	addEntity(&ship.shape);

	for(int i = 0; i < boulders.size(); ++i) {
		for(int j = 0; j < boulders[i]->shape.convexShapes.size(); ++j) {
			removeEntity(boulders[i]->shape.convexShapes[j]);
		}
		delete boulders[i];
	}
	boulders.clear();
	for(int i = 0; i < missiles.size(); ++i) {
		removeEntity(&missiles[i]->shape);
		delete missiles[i];
	}
	missiles.clear();

	for(int i = 0; i < explosions.size(); ++i) {
		removeEntity(&explosions[i]->sprite);
		delete explosions[i];
	}
	explosions.clear();

	while(!lifeIcons.empty()) lifeIcons.pop();
	for(int i = 0; i < lives; ++i) {
		sf::Sprite *icon = new sf::Sprite;
		icon->setTexture(lifeIconTexture);
		icon->setOrigin(icon->getLocalBounds().width, 0.0f);
		icon->setPosition(getWindowWidth() - icon->getLocalBounds().width * lifeIcons.size(), 0.0f);
		lifeIcons.push(icon);
		addEntity(icon);
	}

	setupLevel();
}

void GameScene::onStart() {
	setupGame();
}

void updateShipAnimationPosition() {

}

void GameScene::hyperspace() {
	int i;
	rect r, * rects = new rect[boulders.size()];

	for(i = 0; i < boulders.size(); ++i) {
		rects[i] = rect(boulders[i]->shape.getLeft(), boulders[i]->shape.getTop(), boulders[i]->shape.getRight(), boulders[i]->shape.getBottom());

	}

	inHyperspace = true;

	shipAnimation = ship;
	ship.canHyperspace = ship.active = false;
	addEntity(&shipAnimation.shape);
	shipAnimation.force = -1;

	r = findBestRect(rects, boulders.size(), getWindowWidth(), getWindowHeight());
	ship.pos.x = (r.left + r.right)/2.0f;
	ship.pos.y = (r.top + r.bottom)/2.0f;

	startTimer(Timer(sf::seconds(1), Function(&ship, &ship::enableHyperspace)));
}

void GameScene::countInvunerable() {
	ship.invunerableTick++;
	ship.transparent = !ship.transparent;
	if(ship.invunerableTick == 10) {
		ship.invunerable = false;
		ship.canShoot = true;
		stopTimer(ship.invunerableTimer);
	}
}

void GameScene::onKeyPressed(sf::Keyboard::Key key) {
	if(key == sf::Keyboard::Key::Up and ship.active) ship.accelerating = true;
	else if(key == sf::Keyboard::Key::Left and ship.active) ship.rotatingleft = true;
	else if(key == sf::Keyboard::Key::Right and ship.active) ship.rotatingright = true;
	else if(key == sf::Keyboard::Key::P and !gameOver) {
		this->paused = !this->paused;
		if(paused) addEntity(&pausedText);
		else removeEntity(&pausedText);
	}
	else if(key == sf::Keyboard::Key::Space) {
		if(ship.active) ship.willShoot = true;
		if(gameOver) {
			gameOver = false;
			setupGame();
		}
	}
	else if(key == sf::Keyboard::Key::Down and ship.active and ship.canHyperspace and !ship.invunerable) hyperspace();
}

void GameScene::onKeyReleased(sf::Keyboard::Key key) {
	if(key == sf::Keyboard::Key::Up) ship.accelerating = false;
	else if(key == sf::Keyboard::Key::Left) ship.rotatingleft = false;
	else if(key == sf::Keyboard::Key::Right) ship.rotatingright = false;
	else if(key == sf::Keyboard::Key::Space) ship.willShoot = false;
}

void GameScene::addBoulderEntity(Asteroid * b) {
	int i;
	for(i = 0; i < b->shape.convexShapes.size(); ++i) {
		addEntity(b->shape.convexShapes[i]);
	}
}

void GameScene::shipUpdate() {
	if(!ship.active or ship.transparent) {
		ship.shape.setFillColor(sf::Color::Transparent);
		ship.fireShape.setFillColor(sf::Color::Transparent);
	}
	else {
		ship.shape.setFillColor(sf::Color::White);
		if(ship.accelerating) ship.fireShape.setFillColor(sf::Color::Red);
		else ship.fireShape.setFillColor(sf::Color::Transparent);
	}

	if(ship.active == false) return;
	if(ship.accelerating) {
		sf::Vector2f dir(cosf(ship.angle), sinf(ship.angle));
		ship.speed += dir*ship.force;
	}
	if(ship.rotatingleft) {
		ship.angle = fmodf(ship.angle - ship.rotationSpeed, 2.0f*PI);
	}
	if(ship.rotatingright) {
		float f = ship.angle + ship.rotationSpeed;
		if(f < 0) f += 2.0f*PI;
		ship.angle = fmodf(f, 2.0f*PI);
	}
	if(!isZero(magnitude(ship.speed))) ship.speed -= versor(ship.speed)*ship.deacceleration*magnitude(ship.speed);
	if(magnitude(ship.speed) > ship.maxSpeed) ship.speed = versor(ship.speed)*ship.maxSpeed;
	ship.pos += ship.speed;
	
	//checks if ship is out of bounds
	if(ship.pos.y > getWindowHeight()) ship.pos.y = 0.0f;
	if(ship.pos.y < 0.0f) ship.pos.y = getWindowHeight();
	if(ship.pos.x > getWindowWidth()) ship.pos.x = 0.0f;
	if(ship.pos.x < 0.0f) ship.pos.x = getWindowWidth();
	
	/* Tenho que adicionar o missel depois de alterar a posicao da nave, pois senao o missil nao vai sair no lugar correto*/
	if(ship.willShoot && ship.canShoot){ 
		missile *m = createMissile();
		missiles.push_back(m);
		addEntity(&m->shape);
		ship.canShoot = false;
		startTimer(ship.waitToShoot);
	}
	
	ship.shape.setPosition(ship.pos);
	ship.fireShape.setPosition(ship.pos);
	ship.shape.setRotation(degrees(ship.angle));
	ship.fireShape.setRotation(degrees(ship.angle));
}

missile* GameScene::createMissile() {
	missile* m = new missile;
	sf::Vector2f dir(cosf(ship.angle), sinf(ship.angle));
	m->pos = ship.pos + dir*13.0f;
	m->speed = dir*m->initialSpeed + 0.5f*ship.speed + dir*dotProduct(0.5f*ship.speed, dir);
	m->shape.setPosition(m->pos);
	m->countdown = Timer(sf::seconds(2), Function(m, &missile::destroy));
	startTimer(m->countdown);
	return m;
}

void GameScene::missilesUpdate() {
	int i;
	for(i = 0; i < missiles.size(); ++i) {
		
		missile* m = missiles[i];
		if(m->isGone) {
			removeEntity(&m->shape);
			missiles.erase(missiles.begin() + i);
			i--;
			delete m;
			continue;
		}
		m->pos += m->speed;
		
		if(m->pos.y > getWindowHeight()) m->pos.y = 0.0f;
		if(m->pos.y < 0.0f) m->pos.y = getWindowHeight();
		if(m->pos.x > getWindowWidth()) m->pos.x = 0.0f;
		if(m->pos.x < 0.0f) m->pos.x = getWindowWidth();
		m->shape.setPosition(m->pos);
	}
}

void GameScene::bouldersUpdate() {
	int i, j;
	
	for(i = 0; i < boulders.size(); ++i) {
		Asteroid * b = boulders[i];

		if(b->isGone) {
			for(j = 0; j < b->shape.convexShapes.size(); ++j) 
				removeEntity(b->shape.convexShapes[j]);
			boulders.erase(boulders.begin() + i);
			i--;
			delete b;
			continue;
		}

		b->setPosition(b->position + b->speed);
		b->setRotation(b->angle + b->rotationSpeed);

		/* Esta completamente fora da janela */
		if(b->shape.getBottom() < 0.0f - eps or b->shape.getTop() > getWindowHeight() + eps or b->shape.getRight() < 0.0f - eps or b->shape.getLeft() > getWindowWidth() + eps) b->isGone = true; 

		/* Esta completamente dentro da janela (horizontalmente) */
		if(b->shape.getRight() <= getWindowWidth() and b->shape.getLeft() >= 0.0f) b->xGhost = NULL;
		if(b->shape.getBottom() <= getWindowHeight() and b->shape.getTop() >= 0.0f) b->yGhost = NULL;

		if(b->shape.getBottom() > getWindowHeight() and !b->yGhost and !b->isGone) {
			if(!b->xGhost or !b->xGhost->yGhost or b->xGhost->yGhost->xGhost == NULL) {
				Asteroid * boulder = b->copy();
				boulder->setPosition(b->position.x, b->position.y - getWindowHeight());
				boulder->yGhost = b;
				if(b->xGhost and b->xGhost->yGhost) {
					b->xGhost->yGhost->xGhost = boulder;
					boulder->xGhost = b->xGhost->yGhost;
				}
				else boulder->xGhost = NULL;
				b->yGhost = boulder;
				boulders.push_back(boulder);
				addBoulderEntity(boulder);
			}
		}

		if(b->shape.getTop() < 0.0f and !b->yGhost and !b->isGone) {
			if(!b->xGhost or !b->xGhost->yGhost or b->xGhost->yGhost->xGhost == NULL) {
				Asteroid * boulder = b->copy();
				boulder->setPosition(b->position.x, b->position.y + getWindowHeight());
				boulder->yGhost = b;
				if(b->xGhost and b->xGhost->yGhost) {
					b->xGhost->yGhost->xGhost = boulder;
					boulder->xGhost = b->xGhost->yGhost;
				}
				else boulder->xGhost = NULL;
				b->yGhost = boulder;
				boulders.push_back(boulder);
				addBoulderEntity(boulder);
			}
		}

		if(b->shape.getLeft() < 0.0f and !b->xGhost and !b->isGone) {
			if(!b->yGhost or !b->yGhost->xGhost or b->yGhost->xGhost->yGhost == NULL) {
				Asteroid * boulder = b->copy();
				boulder->setPosition(b->position.x + getWindowWidth(), b->position.y);
				boulder->xGhost = b;
				if(b->yGhost and b->yGhost->xGhost) {
					b->yGhost->xGhost->yGhost = boulder;
					boulder->yGhost = b->yGhost->xGhost;
				}
				else boulder->yGhost = NULL;
				b->xGhost = boulder;
				boulders.push_back(boulder);
				addBoulderEntity(boulder);
			}
		}

		if(b->shape.getRight() > getWindowWidth() and !b->xGhost and !b->isGone) {
			if(!b->yGhost or !b->yGhost->xGhost or b->yGhost->xGhost->yGhost == NULL) {
				Asteroid * boulder = b->copy();
				boulder->setPosition(b->position.x - getWindowWidth(), b->position.y);
				boulder->xGhost = b;
				if(b->yGhost and b->yGhost->xGhost) {
					b->yGhost->xGhost->yGhost = boulder;
					boulder->yGhost = b->yGhost->xGhost;
				}
				else boulder->yGhost = NULL;
				b->xGhost = boulder;
				boulders.push_back(boulder);
				addBoulderEntity(boulder);
			}
		}
	}

	if(boulders.size() == 0 and betweenLevels == false and gameOver == false) {
		level++;
		betweenLevels = true;
		startTimer(waitToChangeLevel);
	}
}

void GameScene::breakAsteroid(Asteroid * a, sf::Vector2f speed) {
	int i;
	sf::Vector2f initSpeed = a->speed;
	sf::Vector2f pos = a->position;
	a->destroy();

	Asteroid * fragments[3];

	if(a->size == 'B') for(i = 0; i< 3; ++i) fragments[i] = new MediumBoulder();
	else if(a->size == 'M') for(i = 0; i< 3; ++i) fragments[i] = new SmallBoulder();
	else return; //small Boulder, nao faz nada

	if(pos.x < 0.0f) pos.x += getWindowWidth();
	if(pos.x >= getWindowWidth()) pos.x -= getWindowWidth();
	if(pos.y < 0.0f) pos.y += getWindowHeight();
	if(pos.y > getWindowHeight()) pos.y -= getWindowHeight();
	for(i = 0; i < 3; ++i) {
		fragments[i]->setPosition(pos);
		fragments[i]->speed += 0.1f*speed + initSpeed;
		boulders.push_back(fragments[i]);
		addBoulderEntity(fragments[i]);
	}

}

void GameScene::respawn() {
	ship.pos = sf::Vector2f(getWindowWidth()/2.0f, getWindowHeight()/2.0f);
	ship.speed = zerovec;
	ship.angle = (3*PI)/2.0f;
	ship.invunerable = true;
	ship.active = true;
	ship.canShoot = false;
	ship.invunerableTick = 0;
	addEntity(&ship.shape);
	addEntity(&ship.fireShape);
	stopTimer(ship.invunerableTimer);
	startTimer(ship.invunerableTimer);
}

void GameScene::collisionChecking() {
	int i, j, k;
	vector<sf::Vector2f> points1, points2;

	for(i = 0; i < missiles.size(); ++i) { //colisoes de misseis com asteroides
		for(j = 0; j < boulders.size(); ++j) {
			if(missiles[i]->isGone) break;
			for(k = 0; k < boulders[j]->shape.absConvexPoints.size(); ++k) { //cada convexShape que forma o concave shape do asteroide
				points1 = boulders[j]->shape.absConvexPoints[k];
				if(isCollidingCirclePolygon(missiles[i]->pos, missiles[i]->radius, points1)) {

					stopTimer(missiles[i]->countdown);
					breakAsteroid(boulders[j], missiles[i]->speed);
					missiles[i]->destroy();

					if(!gameOver) {
						int temp = points/NEWLIFE_SCORE;

						explosion * exp = new explosion();
						explosions.push_back(exp);
						exp->sprite.setTexture(explosionTexture);
						exp->sprite.setOrigin(exp->sprite.getLocalBounds().width/2.0f, exp->sprite.getLocalBounds().height/2.0f);
						exp->sprite.setPosition(missiles[i]->pos);

						addEntity(&exp->sprite);

						if(boulders[j]->size == 'B') points += BIGBOULDER_SCORE;
						else if(boulders[j]->size == 'M') points += MEDIUMBOULDER_SCORE;
						else points += SMALLBOULDER_SCORE;
						if(points/NEWLIFE_SCORE > temp) {
							++lives;
							sf::Sprite *icon = new sf::Sprite;
							icon->setTexture(lifeIconTexture);
							icon->setOrigin(icon->getLocalBounds().width, 0.0f);
							icon->setPosition(getWindowWidth() - icon->getLocalBounds().width * lifeIcons.size(), 0.0f);
							lifeIcons.push(icon);
							addEntity(icon);
						}
					}
					break;
				}
			}
		}
	}

	if(!ship.invunerable and ship.active) {
		points1.clear();
		for(i = 0; i < 3; ++i)
			points1.push_back(ship.shape.getTransform().transformPoint(ship.shape.getPoint(i)));

		for(i = 0; i < boulders.size(); ++i) {
			if(boulders[i]->isGone) continue;
			for(j = 0; j < boulders[i]->shape.absConvexPoints.size(); ++j) {
				points2 = boulders[i]->shape.absConvexPoints[j];
				if(isColliding(points1, points2)) {
					lives--;
					removeEntity(lifeIcons.top());
					delete lifeIcons.top();
					lifeIcons.pop();
					printf("YES\n");
					ship.active = false;


					if(lives) {
						startTimer(ship.waitToRespawn);
					}
					else {
						sf::FloatRect rect;
						gameOver = true;
						char string[100];
						sprintf(string, "%d", points);
						
						finalScoreText.setString(string);
						rect = finalScoreText.getLocalBounds();
						finalScoreText.setOrigin(rect.width/2.0f, rect.height);
						finalScoreText.setPosition(getWindowWidth()/2.0f, getWindowHeight()/2.0f - 15);

						sprintf(string, "BEST: %d", highscore);
						finalHighscoreText.setString(string);
						finalHighscoreText.setPosition(getWindowWidth()/2.0f, getWindowHeight()/2.0f + 53);
						rect = finalHighscoreText.getLocalBounds();
						finalHighscoreText.setOrigin(rect.width/2.0f, 0);


						addEntity(&finalHighscoreText);
						addEntity(&finalScoreText);
						addEntity(&playAgainText);
					}
					removeEntity(&ship.fireShape);
					removeEntity(&ship.shape);

					explosion * exp = new explosion();
					explosions.push_back(exp);
					exp->sprite.setTexture(explosionTexture);
					exp->sprite.setOrigin(exp->sprite.getLocalBounds().width/2.0f, exp->sprite.getLocalBounds().height/2.0f);
					exp->sprite.setPosition(ship.pos);

					addEntity(&exp->sprite);

					break;
				}
			}

		}
	}
}

void GameScene::HUDUpdate() {
	char str[100];
	sprintf(str, "%d", points);
	pointsText.setString(str);
	sf::FloatRect rect = pointsText.getLocalBounds();
	pointsText.setOrigin(rect.width/2.0f, 0.0f);
	pointsText.setPosition(getWindowWidth()/2.0f, 5.0f);
}

void GameScene::explosionsUpdate() {
	int i;
	for(i = 0; i < explosions.size(); ++i) {
		if(explosions[i]->transparency < 0) {
			removeEntity(&explosions[i]->sprite);
			explosions.erase(explosions.begin() + i);
			i--;
			continue;
		}
		explosions[i]->sprite.setColor(sf::Color(255, 255, 255, (int)(255*explosions[i]->transparency)));
		explosions[i]->sprite.setScale(explosions[i]->scale, explosions[i]->scale);
		explosions[i]->transparency -= 0.15f;
		explosions[i]->scale += 0.1f;

	}
}

void GameScene::animationUpdate() {
	if(shipAnimation.force < 0) { //desaparecendo
		if(shipAnimation.shape.getScale().x <= 0.0f) {
			shipAnimation.force = 1.0f;
			shipAnimation.shape.setPosition(ship.pos);
		}
		else {
			shipAnimation.shape.setScale(shipAnimation.shape.getScale() - sf::Vector2f(0.25f, 0.25f));
		}

	}
	else {
		if(shipAnimation.shape.getScale().x >= 0.75f) ship.active = true;
		if(shipAnimation.shape.getScale().x >= 1.0f) {
			inHyperspace = false;
			removeEntity(&shipAnimation.shape);
		}
		else {
			shipAnimation.shape.setScale(shipAnimation.shape.getScale() + sf::Vector2f(0.25f, 0.25f));
		}
	}
}

void GameScene::update() {
	missilesUpdate();
	shipUpdate();
	if(inHyperspace) animationUpdate();
	bouldersUpdate();
	explosionsUpdate();
	HUDUpdate();
	collisionChecking();
}

GameScene::~GameScene() {
	int i;
	for(i = 0; i < missiles.size(); ++i) {
		delete missiles[i];
	}
}
