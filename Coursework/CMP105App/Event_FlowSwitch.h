#pragma once
#include "Event.h"
#include "RoomMap.h"
#include "VisualEffect.h"

class Event_FlowSwitch :
	public Event
{
	RoomMap* linkedRoom;                            //room that will be filled with sand after activation
	std::vector<VisualEffect*> linkedRoomEffects;   //container for visual effects in the linked room
	std::vector<VisualEffect*> originRoomEffects;   //container for visual effects in the room where this event is

public:
	Event_FlowSwitch(TileSet* tileSet, sf::Texture* texture, AudioManager* audioManager) :                                                                                             // needs th eparameters that are not constants or exist elsewhere
		Event(tileSet, 77, 78, std::vector<DirectionState> {DirectionState::UP, DirectionState::RIGHT, DirectionState::DOWN, DirectionState::LEFT}, texture, true, audioManager) {}    // passes the type constant values into the base class constructor
	~Event_FlowSwitch();

	void collisionResponse(GameObject* collider) override;                                                                               //specific collision response 
	void setLinkedRoom(RoomMap* linkedRoom, std::vector<VisualEffect*> linkedRoomEffect, std::vector<VisualEffect*> originRoomEffects);  //set the linked room as the base class does not have this
};

