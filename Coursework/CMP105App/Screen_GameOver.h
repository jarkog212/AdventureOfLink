#pragma once
#include "Screen.h"

class Screen_GameOver : Screen
{
public:
	Screen_GameOver(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager, sf::Font* F_base);   //constructor
	~Screen_GameOver();

	void handleInput() override;           //overrides the handle input
	void screenUpdate(float dt) override;  //overrides the update
	void render() override;                //overrides the render
};

