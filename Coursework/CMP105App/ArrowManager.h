#pragma once
#include "Arrow.h"

// holds and manages all the arrows and shootsthem if it can, if it cannot, it does nothing
class ArrowManager
{
	int numOfArrows = 5;       // max number of shot arrows at a time
	Arrow* quiver[5];          // array tht holds all the arrow objects
	sf::Texture T_arrow;       // the texture that all the arrows point to
	sf::RenderWindow* window;  // window pointer for easier rendering

public:
	ArrowManager();            //constructor
	~ArrowManager();

	void shootArrow(sf::Vector2f playerCenter, sf::Vector2f mousePosition);   //finds the non alive, non shot arrow, prepares it and the n shoots it
	void update(float dt);                                                    // calls the update for individual arrow objects
	void setWindow(sf::RenderWindow* window) { this->window = window; }       // seits the pointer to the window

	std::vector<Arrow*> getAliveArrows();                                     // returns a vector of pointers to the arrows that are alive only
};

