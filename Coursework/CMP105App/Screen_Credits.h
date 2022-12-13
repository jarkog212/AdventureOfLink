#pragma once
#include "Screen.h"

class Screen_Credits : Screen
{

public:
	Screen_Credits(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager, sf::Font* F_base, sf::Texture* T_background);  //constructor
	~Screen_Credits();

	void handleInput() override;            //overrides the handle input
	void screenUpdate(float dt) override;   //overrides the update 
	void render() override;                 //overrides the render
};


