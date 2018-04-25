#include "include/GameManager.hpp"
#include "include/Scene.hpp"
#include "include/SplashScene.hpp"
#include "include/MainMenuScene.hpp"
#include "include/GameScene.hpp"

GameManager * createGame() {
	GameManager *gameManager = new GameManager;
	
	gameManager->setWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	return gameManager;
}

int main(int argc, char * argv[]) {
	GameManager * gameManager = createGame();
	
	/* Scenes */
	SplashScene * splashScreen = new SplashScene(WINDOW_WIDTH, WINDOW_HEIGHT);
	MainMenuScene * mainMenuScene = new MainMenuScene(WINDOW_WIDTH, WINDOW_HEIGHT);
	GameScene gameScene(WINDOW_WIDTH, WINDOW_HEIGHT);

	
	gameManager->runScene(splashScreen);
	delete splashScreen;
	gameManager->runScene(mainMenuScene);
	delete mainMenuScene;
	gameManager->runScene(&gameScene);
	gameManager->finishGame();
	delete gameManager;
    return 0;
}
