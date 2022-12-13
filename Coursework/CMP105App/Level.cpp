#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gameState, AudioManager* audioManager)
{
	//initialise the pointers
	window = hwnd;
	input = in;
	this->gameState = gameState;
	this->audioManager = audioManager;

	// initialise game objects
	player = new Link(audioManager);                 //creates player on heap
	player->setPosition(500, 320);
	player->setInput(input);
	player->parseWindow(window);

	roomManager = new RoomLayouts(in, audioManager); //creates roomManager on heap
	roomManager->setEventInput(in);

	ui = new HUD(window,player);                     //creates HUD on heap
}

Level::~Level()
{
	//deletes all the objects on heap, calls their destructor which destroys thier objects on heap
	delete player; 
	delete roomManager;
	delete ui;
}

// handle user input
void Level::handleInput(float dt)
{
	player->handleInput(dt);                                                             //handles the input for the player, by calling the player's handle input
	if (input->isKeyDown(sf::Keyboard::Enter)) {                                         //if menu button is pressed...
		audioManager->getMusic()->setVolume(audioManager->getMusic()->getVolume() / 2);  //...lowers the volume to half...
		audioManager->playSoundbyName("menuButtonSFX");                                  //...plays sfx...
		input->setKeyUp(sf::Keyboard::Enter);                                     
		gameState->setCurrentState(State::PAUSE);                                        //...changes state to show map menu
	}
}

// Update game objects
void Level::update(float dt)
{
	roomManager->collision(player);                                     //checks on collision, uses room manager that holds all the other manager's calls
	player->update(dt);                                                 //updates the player
	collision();                                                        //calls the remaining collision checks (arrows)
	roomManager->update(dt,player);                                     //calls the update for rooms, which also calls the update for the rest of the objects

	roomManager->setCurrentRoom(player->getCurrentLocation().y);        //sets the current room, to render as well as use objects for from the players location values

	handleView();                                                       //sets the view with the player at the center
	ui->update();                                                       //updates the ui, calls their update which handles itself
	
	if (player->getHealth() <= 0) {                                     //determines the behaviour based  on player health. if less than 0...
		if (abs(player->getHealth() + 1.33f) < 0.00001f) {              //...and specifically -1.33 (unachiavable by normal means)...                    -float check-
			gameState->setCurrentState(State::MENU);                    //...changes the game screen/state to main menu, indirectly also deleting itself to recreate the level...
			audioManager->stopAllMusic();                               //...changes the music to be the menu theme.
			audioManager->playMusicbyName("menuBGM");
		}
		else if (abs(player->getHealth() + 1.11f) < 0.00001f) {         //...and or specifically -1.11 (unachiavable by normal means)...                 -float check-
			gameState->setCurrentState(State::CREDITS);                 //...sets tehe screen/game state to be credits...
			audioManager->stopAllMusic();                               //...changes the music to credits theme
			audioManager->playMusicbyName("creditsBGM");
		}
		else
		{
			gameState->setCurrentState(State::GAMEOVER);                //... otherwise the player is dead and show the game over screen
			audioManager->stopAllMusic();
		}
		resetView();
	}
}

// Render level
void Level::render()
{
	beginDraw();
	// renders everything in the proper order...
	roomManager->renderRoom(window, 0);        //...1st layer of the rooms...
	roomManager->renderRoom(window, 1);        //...2nd layer of the rooms...
	roomManager->renderDoors(window);          //...doors... 
	roomManager->renderEvents(window);         //...events...
	roomManager->renderEnemies(window);        //...enemies...
	window->draw(*player);                     //...player...
	roomManager->renderRoom(window, 2);        //...3rd layer of the room...
	roomManager->renderEffects(window);        //...visual effects...
	renderArrows();                            //...arrows...
	ui->renderHUD();                           //...and lastly, the HUD
	endDraw();
}

void Level::resetView()
{
	sf::View view = window->getView();                                    //gets the current view
	view.setCenter(window->getSize().x / 2, window->getSize().y / 2);     //sets the center to be th eoriginal view, i.e. makes th eorigin of 0,0 match for view and world
	window->setView(view);                                                //applies these changes
}

void Level::renderArrows()
{
	std::vector<Arrow*> arrowsToRender = player->getAliveArrows();        //gets only the arrows that need to be rendered
	for (int i = 0; i < arrowsToRender.size(); i++) {                     //renders them all 
		window->draw(*arrowsToRender[i]);
	}
}

void Level::collision()        //collision for arrows
{
	// stores the vectors of object pointers in the current room for reduced access time
	std::vector<GameObject>* currentLevel = roomManager->getCurrentRoom(0)->getLevel();
	std::vector<Enemy*> currentEnemies = roomManager->getEnemiesInTheRoom();
	std::vector<Arrow*> currentArrows = player->getAliveArrows();

	for (int i = 0; i < currentArrows.size(); i++) {                                      //iterate through all the arrows
		for (int j = 0; currentArrows[i]->isAlive() && j < currentLevel->size(); j++) {   //iterate through all the tiles if the current arrow is alive (shot)
			if ((*currentLevel)[j].isCollider() &&                                        //if the current tile is a collider and...
				!(*currentLevel)[j].isAlive() &&                                          //...if the current tile is NOT alive (alive is used for semi collision in the context of the tiles) and...
				currentArrows[i]->isCollider() &&                                         //...if the arrow is a collider (i.e. is still flying) and...
				Collision::checkBoundingBox(&(*currentLevel)[j], currentArrows[i]))       //...if bothe the tile and arrow collide then...
			{
				currentArrows[i]->wallCollisionResponse();                                //...call the arrow's wall collision response
			}
		}
	}
	for (int i = 0; i < currentArrows.size(); i++) {                                                                    //iterate through all the arrows
		for (int j = 0; currentArrows[i]->isAlive() && j < currentEnemies.size(); j++) {                                //iterate through all the enemies if the arrow is alive (i.e. shot)
			if (currentEnemies[j]->isAlive() && Collision::checkBoundingBox(currentEnemies[j], currentArrows[i])) {     //if the enemy is alive and they both collide then...
				currentEnemies[j]->isHit();                                                                             //... call the enemy damage collision response...
				currentArrows[i]->collisionResponse(NULL);                                                              //... call the arrow's standard collision response...
				audioManager->playSoundbyName("enemyHitSFX");                                                           //... play sfx
			} 
		}
	}
}

// Begins rendering to the back buffer. Background colour set.
void Level::beginDraw()
{
	window->clear(sf::Color(75, 50, 0));
}

// Ends rendering to the back buffer, and swaps buffer to the screen.
void Level::endDraw()
{
	window->display();
}

void Level::handleView()
{
	sf::View view = window->getView();                                                                         //get the current view
	sf::Vector2f center = sf::Vector2f(floor(player->getCenterPoint().x), floor(player->getCenterPoint().y));  //set the player position to be the center
	view.setCenter(center);                                                                                    
	window->setView(view);                                                                                     //apply the view changes
}
