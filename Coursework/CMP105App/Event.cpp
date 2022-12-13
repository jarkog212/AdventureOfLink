#include "Event.h"

Event::Event(TileSet* tileSet, int activeTile, int inactiveTile, std::vector<DirectionState> interactableDirections, sf::Texture* texture, bool isSolid, AudioManager* audioManager)
{
	//initialise pointers
	this->audioManager = audioManager;

	for (int i = 0; i < interactableDirections.size(); i++)  //change the format of interactable direction
	{
		switch (interactableDirections[i]) {
		case DirectionState::UP:
			interactionDirection += 0b00000001;
			break;
		case DirectionState::RIGHT:
			interactionDirection += 0b00000010;
			break;
		case DirectionState::DOWN:
			interactionDirection += 0b00000100;
			break;
		case DirectionState::LEFT:
			interactionDirection += 0b00001000;
			break;
		}
	}

	stateInactive = tileSet->getTileSet()[inactiveTile].getTextureRect();                                          //set the texture rectangle based on tile input and tileset 
	stateActive = tileSet->getTileSet()[activeTile].getTextureRect();                                              //same as above
	setTexture(texture);                                                                                           //set the texture
	setCollider(isSolid);                                                                                          //whethe rpassable by the player
	setTextureRect(stateActive);                                                                                   //set active texture
	setSize(tileSet->getTileSize() * tileSet->getScale());                                                         //set size to match the texture * scale
	setCollisionBox(0, 0, getSize().x, getSize().y);                                                               //set standard collision box to match the size
	solidCollisionBox = sf::FloatRect(getSize().x / 6, getSize().y / 6, getSize().x * 4 / 6, getSize().y * 4 / 6); //set the solid collision box to be smaller than standard collision box
}

Event::~Event()
{
}


void Event::interact(Link* collider)
{
	if (((collider->getDirection() == DirectionState::UP && (interactionDirection & 0b00000001)) ||             // if the interactable direction matches the current direction of the player and the event is interactable...
		(collider->getDirection() == DirectionState::RIGHT && (interactionDirection >> 1 & 0b00000001)) ||
		(collider->getDirection() == DirectionState::DOWN && (interactionDirection >> 2 & 0b00000001)) ||
		(collider->getDirection() == DirectionState::LEFT && (interactionDirection >> 3 & 0b00000001))) &&
		interactable) 
	{
		collisionResponse(collider);                                                                            // call the standard collision response                   
	}
}

void Event::collisionResponse(GameObject* collider)            // dummy collision response
{
	interactable = false;                              
	setTextureRect(stateInactive);
}


