#pragma once
#include "Link.h"
#include "Screen.h"

class Screen_MenuMap : Screen
{
	sf::Texture T_MapF1;                                          //texture for Floor 1 map
	sf::Texture T_LinkIcon;                                       //texture for link icon
	sf::RectangleShape F1Map;                                     //object carrying the texture
	sf::RectangleShape linkIcon;                                  //object carrying the link icon
	Link* player;                                                 //pointer to the player
	std::vector<std::vector<sf::Vector2i>> relativeIconLocations; //2D vector holds the icon locations [floor][room]

	void setRelativeIconLocations();                              //sets the location based on the player location values
	void setPosition();                                           //sets the positions of objects

public:
	Screen_MenuMap(sf::RenderWindow* window, Input* input, Link* player, GameState* gameState, AudioManager* audioManager, sf::Font* F_base, sf::Texture* T_background);  //constructor
	~Screen_MenuMap();

	void handleInput() override;                               //overrides the handle input
	void screenUpdate(float dt) override;                      //overrides the update
	void render() override;                                    //overrides the render
	void setPlayer(Link* player) { this->player = player; }    //gets and initialises the pointer to the player (needed after reset)

};

