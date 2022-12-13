#include "Screen_GameOver.h"
#define TEXT_POS_X 450/4
#define TEXT_POS_Y 600



Screen_GameOver::Screen_GameOver(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager, sf::Font* F_base):
	Screen(window,input,gameState,audioManager,F_base,NULL) //parses the values into base constructor
{
	//initialise the text
	text.setString("Game Over");
	text.setCharacterSize(70);
	text.setPosition(TEXT_POS_X, TEXT_POS_Y);
	text.setFillColor(sf::Color(255,0,0,255));
}

Screen_GameOver::~Screen_GameOver()
{
}

void Screen_GameOver::handleInput()
{
	if(input->isKeyDown(sf::Keyboard::Enter)) {            //if the menu key is pressed then... 
		input->setKeyUp(sf::Keyboard::Enter);
		audioManager->playSoundbyName("menuButtonSFX");    //...play sfx...
		audioManager->playMusicbyName("menuBGM");          //...change music...
		gameState->setCurrentState(State::MENU);           //...change the screen to menu
	}
}

void Screen_GameOver::screenUpdate(float dt)
{
	makeTextBlink(dt);                           // makes the text blink
}

void Screen_GameOver::render()
{
	//draws the objects
	beginDraw();
	window->draw(text);
	endDraw();
}
