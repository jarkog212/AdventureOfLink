#pragma once
#include "Event.h"
#include "Door.h"
class Event_KeyHole :
	public Event
{
	Door* linkedDoor;      // door that will be unlocked from this keyhole
public:
	Event_KeyHole(TileSet* tileSet, sf::Texture* texture, AudioManager* audioManager) :                                                                                              // needs th eparameters that are not constants or exist elsewhere
		Event(tileSet, 83, 82, std::vector<DirectionState> {DirectionState::UP, DirectionState::RIGHT, DirectionState::DOWN, DirectionState::LEFT}, texture, true, audioManager) {}  // passes the type constant values into the base class constructor
	~Event_KeyHole();

	void collisionResponse(GameObject* collider) override;                   //specific collision response 
	void setLinkedDoor(Door* linkedDoor) { this->linkedDoor = linkedDoor; }  // since base class misses the linked door it needs to be set manually
};

