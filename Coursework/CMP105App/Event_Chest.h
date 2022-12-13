#pragma once
#include "Event.h"
class Event_Chest :
	public Event
{
public:
	Event_Chest(TileSet* tileSet, sf::Texture* texture, AudioManager* audioManager):                                 // needs th eparameters that are not constants or exist elsewhere
		Event(tileSet, 53, 51, std::vector<DirectionState> {DirectionState::UP}, texture, true, audioManager) {};    // passes the type constant values into the base class constructor
	~Event_Chest();

	void collisionResponse(GameObject* collider) override;      //specific collision response 
};

