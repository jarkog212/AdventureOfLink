#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/Input.h"
#include "Framework/AudioManager.h"
#include "Framework/GameState.h"
#include "Framework/Collision.h"
#include "RoomLayouts.h" 
#include "HUD.h"
#include "Link.h" 
#include <string>
#include <iostream>

// combines the room manager (RoomLayouts) with the Player (Link) functionalities, calls updates and renders objects in the proper order

class Level{
public:
	Level(sf::RenderWindow* hwnd, Input* in, GameState* gameState, AudioManager* audioManager);     //constructor
	~Level();

	void handleInput(float dt);                                          // method to call other objects handle inputs as well as handle the menu calling input
	void update(float dt);                                               // call the other objects' update as well as collision
	void render();                                                       // renders and calls the available ender functions
	Link* getPlayerRef() { return player; }
	void setHUDfont(sf::Font* F_base) { ui->setHUDfont(F_base); }        // sets the font for the HUD
	void resetView();                                                    // returns view so that the origin is 0,0 for both view and world location
	void renderArrows();                                                 // renders all the arrows from the player (gets only the alive ones)
	void collision();                                                    // handles some collision, arrows and calls the rest of the collision handlers within the managers

private:
	// Default functions for rendering to the screen.
	void beginDraw();
	void endDraw();
	void handleView();
	
	// Default pointers for level class.
	sf::RenderWindow* window;
	Input* input;
	GameState* gameState;
	AudioManager* audioManager;
	RoomLayouts* roomManager;
	Link* player;
	HUD* ui;
};