#pragma once
#include "FrameWork/GameObject.h"
#include "HUDe_Heart.h"
#include "HUDe_WeaponType.h"
#include "Link.h"

class HUD
{
	sf::RenderWindow* window;       // pointer to the window for easier rendering
	Link* player;                   // pointer to the player for access to values
	sf::Texture T_heartsAndKey;     // texture for hearts and key
	sf::Texture T_weaponType;       // teaxture for weapon types (only Sword and Bow now)
	sf::Texture T_bar;              // texture for the HUD bar
	std::vector<HUDe_Heart> hearts; // vector holdin all the hearts 
	int numOfHearts;                // number of maximum hearts 
	sf::RectangleShape bar;         // rectangle holding the texture for the bar
	sf::RectangleShape key;         // rectangle holdin the key texture
	sf::Text keyNumber;             // text showing the current number of keys to the player
	HUDe_WeaponType* weaponType;    // Hud element weapon type

	void setPosition();             // function to properly set the position of the entire HUD as it moves with the view

public:
	HUD(sf::RenderWindow* window, Link* player); //constructor
	~HUD();

	void renderHUD();                                                 //renders HUD
	void update();                                                    //updates all the elements
	void updateHealth();                                              //updates the hearts 
	void setHUDfont(sf::Font* F_base) { keyNumber.setFont(*F_base); } //sets the hud font from a pointer

};

