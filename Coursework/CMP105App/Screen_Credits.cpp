#include "Screen_Credits.h"
//text specific constants
#define TEXT_POS_X 20
#define TEXT_POS_Y 840

Screen_Credits::Screen_Credits(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager, sf::Font* F_base, sf::Texture* T_background):
	Screen(window,input,gameState,audioManager,F_base,T_background) // parses the neccesary parameters into the base class' constructor
{
	//initialise the text
	text.setString("To go back press Enter");
	text.setCharacterSize(40);
	text.setPosition(TEXT_POS_X, TEXT_POS_Y);
	text.setFillColor(sf::Color(0, 0, 0, 255));
}

Screen_Credits::~Screen_Credits()
{
}

void Screen_Credits::handleInput()
{
	if (input->isKeyDown(sf::Keyboard::Enter))          //if the menu key is pressed then... 
	{
		input->setKeyUp(sf::Keyboard::Enter);
		gameState->setCurrentState(State::MENU);        //...change the screen to menu...
		audioManager->playSoundbyName("menuButtonSFX"); //...play sfx...
		audioManager->stopAllMusic();                   //...change music
		audioManager->playMusicbyName("menuBGM");
	}
}

void Screen_Credits::screenUpdate(float dt)
{
	makeTextBlink(dt);                                  // makes the text blink
}

void Screen_Credits::render()
{
	//render all elements
	beginDraw();
	window->draw(background);
	window->draw(text);
	endDraw();
}
