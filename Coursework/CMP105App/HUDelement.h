#pragma once
#include "FrameWork/GameObject.h"
#include "Link.h"

class HUDelement :
	public GameObject
{
protected:
	std::vector<sf::IntRect> states;                     //different states for the object, graphically speaking, could be hearts fill rate or weapon type
	Link* player;                                        //pointer to the player to get the data
	sf::Color C_transparent = sf::Color(0, 0, 0, 0);     //transparent color daved
	sf::Color C_opaque = sf::Color(255, 255, 255, 255);  //opaque color saved
public:
	HUDelement(sf::Texture* texture, Link* player, sf::Vector2i textureCellDimensions, int numOfStates, float textureScale);   //base constructor
	~HUDelement();

	virtual void update(float dt) override;              //overrideable method for each element
};

