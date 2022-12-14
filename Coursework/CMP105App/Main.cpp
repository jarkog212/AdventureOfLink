// Main
// Entry point of the application.
// Handles window creation and window events.
// Contains the game loop, updates input class and contains the level objects.
// @author Paul Robertson

#include <iostream>
#include "Level.h"
#include <windows.h>
#include "Screen_MenuMap.h"
#include "Screen_MainMenu.h"
#include "Screen_GameOver.h"
#include "Screen_Credits.h"
#include "Framework/AudioManager.h"

void initialise(sf::RenderWindow* window, Input* input, GameState* gameState, sf::Font* F_base, Level** level, AudioManager* audioManager);                          //function to intiialise level and put the player from level into the menumap
void restart(sf::RenderWindow* window, Input* input, GameState* gameState, sf::Font* F_base, Level** level, Screen_MenuMap* menuMap, AudioManager* audioManager);    //function o delete the level instance (easy restart) and 

// replacement for standard main(), learned from a tutorial to make it windows app only
int CALLBACK WinMain(
_In_ HINSTANCE hInstance,
_In_ HINSTANCE hPrevInstance,
_In_ LPSTR     lpCmdLine,
_In_ int       nCmdShow)
//int main()
{
	//Create the window, set th eresolution and mode to be unchangable as it is easier to handle
	sf::RenderWindow window(sf::VideoMode(1600,900), "The Legend Of Link: Trials of the Pyramid ", sf::Style::Close );

	// Initialise input and level objects.
	Input input;
	
	sf::Font F_base;                             //font object to be used by various texts, its here as this is the only instance that every other class that needs text points to 
	F_base.loadFromFile("font/Triforce.ttf");    //special font downloaded

	sf::Texture T_mainMenu;                           //texture for main menu, to be used by Menu screen class
	T_mainMenu.loadFromFile("gfx/TitleScreen.png");

	sf::Texture T_credits;                            //texture for credits, to be used by credits screen class
	T_credits.loadFromFile("gfx/CreditsScreen.png");

	sf::Texture T_menuMap;                            //texture to by useed by the pause menu screen class
	T_menuMap.loadFromFile("gfx/MapMenuBG.png");
	
	GameState gameState;                              //main game state class 
	gameState.setCurrentState(State::MENU);           //starts at main menu
	
	AudioManager audioManager;                                       //audio manager class object is in the main so that much like font, everything that needs it points to this
	audioManager.addMusic("bgm/MenuBGM.ogg", "menuBGM");             //all the sounds and music is added here, to be called separately within other classes
	audioManager.addMusic("bgm/PyramidBGM1.ogg", "levelBGM");
	audioManager.addMusic("bgm/EndBGM.ogg", "creditsBGM");
	audioManager.addSound("sfx/EnemyDamaged.ogg", "enemyHitSFX");
	audioManager.addSound("sfx/ItemGet.ogg", "chestOpenSFX");
	audioManager.addSound("sfx/LinkAttack.ogg", "swordSFX");
	audioManager.addSound("sfx/LinkDamaged.ogg", "playerHitSFX");
	audioManager.addSound("sfx/PressStart.ogg", "menuButtonSFX");
	audioManager.addSound("sfx/SandFall.ogg", "sandfallSFX");
	audioManager.addSound("sfx/Secret.ogg", "secretSFX");
	audioManager.addSound("sfx/BowShot.ogg", "bowSFX");
	audioManager.addSound("sfx/WeaponSwitch.ogg", "weaponSwitchSFX");
	audioManager.addSound("sfx/Walk.ogg", "walkSFX");
	audioManager.playMusicbyName("menuBGM");                          //starts with the main menu theme as Menu is the first screen

	Screen_MainMenu mainMenu = Screen_MainMenu(&window, &input, &gameState, &audioManager, &F_base, &T_mainMenu );	  //creates the main menu screen object, uses inheritance form the base Screen class
	Screen_GameOver gameOverScreen = Screen_GameOver(&window, &input, &gameState, &audioManager, &F_base);	          //creates the game over screen object, uses inheritance form the base Screen class
	Screen_Credits creditsScreen = Screen_Credits(&window, &input, &gameState, &audioManager, &F_base, &T_credits);   //creates the credits screen object, uses inheritance form the base Screen class

	Level* level;                                                                // the level pointer, level is created dynamically for easier restarts
	initialise(&window, &input, &gameState, &F_base, &level, &audioManager);     // creates the level at the level pointer, hence pointer to pointer, uses all the other objects needed to create the level

	Screen_MenuMap menuMap = Screen_MenuMap(&window, &input, level->getPlayerRef(), &gameState, &audioManager, &F_base, &T_menuMap);  ////creates the credits screen object, uses inheritance form the base Screen class, since level exists now (and the player within it)
	
	// Initialise objects for delta time
	sf::Clock clock;
	float deltaTime;

	// Game Loop
	while (window.isOpen())
	{
		// Handle window events.
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)event.size.width, (float)event.size.height)));
				break;
			case sf::Event::KeyPressed:
				// update input class
				input.setKeyDown(event.key.code);
				break;
			case sf::Event::KeyReleased:
				//update input class
				input.setKeyUp(event.key.code);
				break;
			case sf::Event::MouseMoved:
				//update input class
				input.setMousePosition(event.mouseMove.x, event.mouseMove.y);
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//update input class
					input.setMouseLDown(true);
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					input.setMouseRDown(true);
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//update input class
					input.setMouseLDown(false);
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					input.setMouseRDown(false);
				}
				break;
			default:
				// don't handle other events
				break;
			}
		}

		// Calculate delta time. How much time has passed 
		// since it was last calculated (in seconds) and restart the clock.
		deltaTime = clock.restart().asSeconds();
		
		// Call standard game loop functions (input, update and render)
		switch (gameState.getCurrentState()) {
		case State::MENU:																			// is the main menu mode, this is also the tutorial screen mode. From this state only level state can be accessed
			if (mainMenu.isReseting()) {															// retart is called when returning from other screens
				restart(&window, &input, &gameState, &F_base, &level, &menuMap, &audioManager);		// resets the level
				mainMenu.disableReset();
			}
			mainMenu.handleInput();																	//based on this, the tutoarial screen is shown or level starts
			mainMenu.screenUpdate(deltaTime);
			mainMenu.render();
			break;
		case State::LEVEL:							// is the level, gameplay mode. from here the game can go to any other state
			level->handleInput(deltaTime);			// handles all the input
			level->update(deltaTime);               // handles all the updates necessary
			level->render();                        // renders all
			break; 
		case State::PAUSE:                          // is the map menu state, from this state only the level state is accessible 
			menuMap.handleInput();
			menuMap.screenUpdate(deltaTime);
			menuMap.render();
			break;
		case State::GAMEOVER:                       // is the game over state, from here only the main menu state is accessible
			gameOverScreen.handleInput();
			gameOverScreen.screenUpdate(deltaTime);
			gameOverScreen.render();
			break;
		case State::CREDITS:                        // is th ecredits state, from here only the main menu state is accessible 
			creditsScreen.handleInput();
			creditsScreen.screenUpdate(deltaTime);
			creditsScreen.render();
		}

	}
	return 0;
}

//initialises the level and sets the hud font in the level
void initialise(sf::RenderWindow* window, Input* input, GameState* gameState, sf::Font* F_base, Level** level, AudioManager* audioManager) {

	*level = new Level(window, input, gameState, audioManager); //creates the level on heap
	(*level)->setHUDfont(F_base);                               //sets the hud font
}

//deletes th elevel an calls the initialisation methiod again, sets the new player for the manuMap (old one is deleted with the level)
void restart(sf::RenderWindow* window, Input* input, GameState* gameState, sf::Font* F_base, Level** level, Screen_MenuMap* menuMap, AudioManager* audioManager)
{
	delete *level;                                                      //deletes the level and all the objects in it
	initialise(window, input, gameState, F_base, level, audioManager);  //recreates the level from scratch
	menuMap->setPlayer((*level)->getPlayerRef());                       //sets the menu map player pointer to the current player
}
