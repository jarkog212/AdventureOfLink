#include "Screen.h"

//standard clear function
void Screen::beginDraw()
{
	window->clear(sf::Color(0, 0, 0, 255));
}

//standard buffer display function
void Screen::endDraw()
{
	window->display();
}

void Screen::makeTextBlink(float dt)
{
	elapsedTime += dt;                                                    //add to elapsed time
	float opacity = abs(sin(elapsedTime));                                //uses sin function to translate the elapsed time to values between 0-1, sin can produce -1 as well hence the abs()            
	int C_red = text.getFillColor().r;                                    //gets the red channel of the current color
	int C_green = text.getFillColor().g;                                  //gets the green channel of the current color
	int C_blue = text.getFillColor().b;                                   //gets the blue channel of the current color
	text.setFillColor(sf::Color(C_red, C_green, C_blue,255 * opacity));   //sets the color of the text using the channels and opacity * sin value output
}

Screen::Screen(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager, sf::Font* F_base, sf::Texture* T_background)
{
	//initialises the pointers
	this->window = window;
	this->input = input;
	this->gameState = gameState;
	this->audioManager = audioManager;
	this->T_background = T_background;
	
	//initialisesthe objects
	text.setFont(*F_base);
	text.setPosition(sf::Vector2f(window->getSize().x/2,window->getSize().y/2));
	background.setSize(sf::Vector2f(window->getSize()));
	background.setTexture(T_background);
	background.setPosition(0, 0);
}

Screen::~Screen()
{
}

void Screen::handleInput()
{
}

void Screen::screenUpdate(float dt)
{
}

void Screen::render()
{
	//renders the objects
	window->draw(background);
	window->draw(text);
}
