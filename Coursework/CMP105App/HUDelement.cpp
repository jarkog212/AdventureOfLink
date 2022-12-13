#include "HUDelement.h"

HUDelement::HUDelement(sf::Texture* texture, Link* player, sf::Vector2i textureCellDimensions, int numOfStates, float textureScale)
{
	//initialise the pointer
	this->player = player;

	setTexture(texture);                                                                                                  //set the texture from the pointer
	setSize(sf::Vector2f(textureCellDimensions.x * textureScale, textureCellDimensions.y * textureScale));                //sets the size to match the texture * scale

	for (int i = 0; i < numOfStates; i++)                                                                                 //procedurally create all the texture rectangles from the number of states
	{
		states.push_back(sf::IntRect(i * textureCellDimensions.x, 0, textureCellDimensions.x, textureCellDimensions.y));
	}

	setTextureRect(states[0]);         // sets the first state as the default one
}

HUDelement::~HUDelement()
{
}

void HUDelement::update(float dt)
{
	// should not do anything by default, a dummy
}
