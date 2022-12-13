#pragma once
#include "Framework/GameObject.h"
#include "FrameWork/AudioManager.h"
#include "Link.h"
#include "TileSet.h"

class Event :
	public GameObject
{
public:

	Event(TileSet* tileSet,int activeTile, int inactiveTile, std::vector<DirectionState> interactableDirections, sf::Texture* texture, bool isSolid, AudioManager* audioManager);  //constructor
	~Event();
	void setInput(Input* input) { this->input = input; }               // set the input pointer, separate because of the input initialisation order 
	void interact(Link* collider);                                     // interact, main function for responding to the player's input, collision
	virtual void collisionResponse(GameObject* collider) override;     // virtual standard collision response, to be determined by the type of the event
	bool isInteractable() { return interactable; }                    
	sf::FloatRect getSolidCollisionBox() { return sf::FloatRect(solidCollisionBox.left + getPosition().x, solidCollisionBox.top + getPosition().y, solidCollisionBox.width, solidCollisionBox.height); 
	}


protected:

	int8_t interactionDirection = 0b00000000; //order from LSB to MSB: UP,RIGHT,DOWN,LEFT
	bool interactable = true;                 //whether the player can interact with th event

	Input* input;                             //input pointer for input based events
	AudioManager* audioManager;               //events produce sound
	sf::IntRect stateActive;                  //texture when is interacteable, i.e. not yet interacted with 
	sf::IntRect stateInactive;                //textur ewhen non interacteble, i.e. already interacted with
	sf::FloatRect solidCollisionBox;          //collision box for wall collision response
};

