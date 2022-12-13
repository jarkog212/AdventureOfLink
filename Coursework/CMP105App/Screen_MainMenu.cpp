#include "Screen_MainMenu.h"
//text specific constants
#define TEXT_POS_X 1000
#define TEXT_POS_Y 425

Screen_MainMenu::Screen_MainMenu(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager, sf::Font* F_base, sf::Texture* T_background) :
Screen(window,input,gameState,audioManager,F_base,T_background)   //parses the values into the base class constructor 
{
	//loads the texture
	T_TutorialScreen.loadFromFile("gfx/TutorialScreen.png");
	//initialises the text
	text.setString("- Press Lshift for tutorial / Enter to play-");
	text.setCharacterSize(30);
	text.setPosition(TEXT_POS_X,TEXT_POS_Y);
	text.setFillColor(sf::Color(0,0,0, 255));
}

Screen_MainMenu::~Screen_MainMenu()
{
}

void Screen_MainMenu::handleInput()
{
	if (input->isKeyDown(sf::Keyboard::Enter))            //if menu key is pressed then...
	{
		input->setKeyUp(sf::Keyboard::Enter);
		gameState->setCurrentState(State::LEVEL);         //...change the game state to level...
		tutorialMode = false;                             //...turns of the turorial flag...
		onNextReset = true;                               //...sets the next reset to true as we want the level to be reset when we get back to the main menu... 
		audioManager->playSoundbyName("menuButtonSFX");   //... play sfx ...
		audioManager->stopAllMusic();                     //... play music
		audioManager->playMusicbyName("levelBGM");
	}
	else if (input->isKeyDown(sf::Keyboard::LShift)) {    //if tutorial switch is pressed then...
		input->setKeyUp(sf::Keyboard::LShift);
		if (!tutorialMode) {                              //...if the turial flag is no t set then...
			background.setTexture(&T_TutorialScreen);     //...sets the turorial texture as background...
			tutorialMode = true;                          //...sets the tutorial flag
		}
		else {
			background.setTexture(T_background);          //otherwise set th etexture to the mainmenu background...
			tutorialMode = false;                         //...disablles th etutorial flag
		}
	
	}
}

void Screen_MainMenu::screenUpdate(float dt)
{
	makeTextBlink(dt);                                    //makes the text blink
}

void Screen_MainMenu::render()
{
	//renders all objects
	beginDraw();
	
	window->draw(background);
	if (!tutorialMode) {
		window->draw(text);
	}
	endDraw();
}

