#pragma once
#include "Event_Button.h"
#include "Event_Chest.h"
#include "Event_FlowSwitch.h"
#include "Event_KeyHole.h"
#include "Framework/Collision.h"

enum class EventType {CHEST,FLOW_SWITCH,BUTTON,KEY_HOLE};    // different types of the events

class EventManager
{
	std::vector<std::vector<std::vector<Event*>>> events;   // 3D vector holding all the events [floor][room][eventID]
	sf::Texture T_tiles;                                    // texture for the tileset
	TileSet* tileSet;                                       // tileset pointer for the room manipulation/ getting texture rectangles by index alone
	Input* input;                                           // input pointer for interaction
	AudioManager* audioManager;                             // audio manager for events to play sounds

public:
	EventManager() {}                                                                                             //dummy constructor
	EventManager(int numOfFloors, int numOfRooms, TileSet* tileSet, Input* input, AudioManager* audioManager);    //main constructor
	~EventManager();

	Event* addEvent(EventType type, int floorIndex, int roomIndex);                                                        //adds event into the proper vector, returns the pointer to the new event
	void collision(Link* player);                                                                                          //handles collision with the player
	void setInput(Input* input) { this->input = input; }                                                                   //sets the input pointer, alone as the input might not yet be initialised
	std::vector<Event*> getEventsInRoom(int currentFloor, int currentRoom) { return events[currentFloor][currentRoom-1]; } //returns the vector of pointers of the events in the current room
};

