#pragma once
#include "Screen.h"

class Screen_MainMenu : Screen
{
	sf::Texture T_TutorialScreen;    //texture for tutorial screen
	bool tutorialMode = false;       //flag for tutorial screen
	bool onNextReset = false;        //whether the level shoul dbe reset when the accessing the main menu next turn

public:
	Screen_MainMenu(sf::RenderWindow* window, Input* input, GameState* gameState, AudioManager* audioManager,sf::Font* F_base, sf::Texture* T_background ); //constructor
	~Screen_MainMenu();

	void handleInput() override;                 //overrides the handle input
	void screenUpdate(float dt) override;        //overrides the update 
	void render() override;                      //overrides the render
	bool isReseting() { return onNextReset; }   
	void disableReset() {onNextReset = false;}
};

