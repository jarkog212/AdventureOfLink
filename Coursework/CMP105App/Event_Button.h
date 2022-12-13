#pragma once
#include "Event.h"
class Event_Button :
	public Event
{
public:
	Event_Button(TileSet* tileSet, sf::Texture* texture, AudioManager* audioManager) :                                                                                               // needs th eparameters that are not constants or exist elsewhere
		Event(tileSet, 97, 96, std::vector<DirectionState> {DirectionState::UP,DirectionState::RIGHT, DirectionState::DOWN, DirectionState::LEFT}, texture, false, audioManager) {}  // passes the type constant values into the base class constructor
	~Event_Button();

	void collisionResponse(GameObject* collider) override; // unfinished
};

