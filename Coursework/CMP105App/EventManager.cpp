#include "EventManager.h"

EventManager::EventManager(int numOfFloors, int numOfRooms, TileSet* tileSet, Input* input, AudioManager* audioManager)
{
	//loads the texture from the tileset
	T_tiles.loadFromFile(tileSet->getTexture());
	//initialises the pointers
	this->tileSet = tileSet;
	this->input = input;
	this->audioManager = audioManager;

	for (int i = 0; i < numOfFloors; i++) {   // properly initialises the 3D vector

		std::vector<std::vector<Event*>> s;
		events.push_back(s);

		for (int j = 0; j < numOfRooms; j++) {
			std::vector<Event*> d;
			events[i].push_back(d);
		}
	}
}

EventManager::~EventManager()
{
	//properly deletes all the events created on heap
	for (int i = 0; i < events.size(); i++) {
		for (int j = 0; j < events[i].size(); j++) {
			for (int k = 0; k < events[i][j].size(); k++) 
			{
				delete events[i][j][k];                                   //deletes the event on heap
			}
			events[i][j].erase(events[i][j].begin(), events[i][j].end()); //deletes the empty vector
		}
		events[i].erase(events[i].begin(), events[i].end());              //deletes the empty vector
	}
	events.erase(events.begin(), events.end());                           //deletes the empty vector
} 

Event* EventManager::addEvent(EventType type, int floorIndex, int roomIndex)      // creates the new event and returns the pointer to it
{
	// creates all types of objects on heap
	Event_Button* b = new Event_Button(tileSet, &T_tiles, audioManager);
	Event_Chest* c = new Event_Chest(tileSet, &T_tiles, audioManager);
	Event_FlowSwitch* f = new Event_FlowSwitch(tileSet, &T_tiles, audioManager);
	Event_KeyHole* k = new Event_KeyHole(tileSet, &T_tiles, audioManager);

	//switch based on the desired type 
	switch (type) {
	case EventType::BUTTON:
		b->setInput(input);                               //sets the input for the desired new event
		events[floorIndex][roomIndex-1].push_back(b);     //saves the address to the proper room vector
		delete c;                                         //deletes the rest of the new events on the heap
		delete f;
		delete k;
		return b;                                         //returns the new event address
		break; 
	case EventType::CHEST:       
		c->setInput(input);                               //same as previous case
		events[floorIndex][roomIndex-1].push_back(c);
		delete b;
		delete f;
		delete k;
		return c;
		break;
	case EventType::FLOW_SWITCH:
		f->setInput(input);                               //same as previous case
		events[floorIndex][roomIndex-1].push_back(f);
		delete b;
		delete c;
		delete k;
		return f;
		break;
	case EventType::KEY_HOLE:
		k->setInput(input);                               //same as previous case
		events[floorIndex][roomIndex-1].push_back(k);
		delete b;
		delete c;
		delete f;
		return k;
		break;
	}
}

void EventManager::collision(Link* player)             // checks different collision with the player
{
	for (int i = 0; i < getEventsInRoom(player->getCurrentLocation().x,player->getCurrentLocation().y).size(); i++)   //iterates through all the events in a single room
	{
		Event* currentEvent = getEventsInRoom(player->getCurrentLocation().x, player->getCurrentLocation().y)[i];     //saves the  current event to reduce access time
		if (currentEvent->isInteractable() && Collision::checkBoundingBox(currentEvent, player))                      //if player collides with the interaction collsion box...
		{ 
			currentEvent->interact(player);                                                                           //...try to interact
 		}
		if (currentEvent->isCollider() && Collision::checkBoundingBox(currentEvent->getSolidCollisionBox(), player))  //if the player collides with the solid collision box...
		{
			player->wallColisionResponse(NULL);                                                                       //...call the player's collision response
		}
	}
}
