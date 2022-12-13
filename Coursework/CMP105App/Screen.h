#pragma once
#include "Framework/GameObject.h"
#include "FrameWork/GameState.h"
#include "Framework/AudioManager.h"
class Screen
{
protected:
	sf::RenderWindow* window;       //pointertothe window, easier for rendering
	Input* input;                   //pointer to the input, to hadnle input
	sf::Texture* T_background;      //texture for background
	sf::RectangleShape background;  //object to carry the background, size is equal to window
	sf::Text text;                  //simple text object if needed 
	GameState* gameState;           //pointer to game state to change the current state 
	AudioManager* audioManager;     //pointer to audio so that th escreen can change the music
	float elapsedTime = 0;          //holds the elapsed time, used for blinking

	void beginDraw();               
	void endDraw();
	void makeTextBlink(float dt);   //updates the texts opacity to use when the text should blink, spearate as a possibility for the choice if some screens don't want the blinking

public:
	Screen(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager, sf::Font* F_base, sf::Texture* T_background);     //constructor
	~Screen();

	virtual void handleInput();              //handles input, virtual so subclasses can override
	virtual void screenUpdate(float dt);     //updates the screen, virtual so sublasseas can override
	virtual void render();                   //renders the screen, virtual so subclasses can override
};

