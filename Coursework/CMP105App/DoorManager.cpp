#include "DoorManager.h"
#include "Framework/Collision.h"
#include <iostream>

DoorManager::DoorManager(int numOfFloors, int numOfRooms, int scale)
{
	//initialises the pointers
	this->scale = scale;
	
	for (int i = 0; i < numOfFloors; i++) {    //initialises the 3D vector properly

		std::vector<std::vector<Door*>> s;
		doors.push_back(s);

		for (int j = 0; j < numOfRooms; j++) {
			std::vector<Door*> d;
			doors[i].push_back(d);
		}
	}
	T_Door.loadFromFile("gfx/Doors.png");      //loads to the texture for doors 
}

DoorManager::~DoorManager()                    //necessary for proper deletion
{
	for (int i = 0; i < doors.size(); i++) {
		for (int j = 0; j < doors[i].size(); j++) {
			for (int k = 0; k < doors[i][j].size(); k++)
			{
				delete doors[i][j][k];         //deletes all the doors on the heap
			}
			doors[i][j].erase(doors[i][j].begin(), doors[i][j].end());    //erases the empty vector
		}
		doors[i].erase(doors[i].begin(), doors[i].end());                 //erases the empty vector
	}
	doors.erase(doors.begin(), doors.end());                              //erases the empty vector 
}

void DoorManager::addDoor(GameObject* referenceTile, DoorSide side, int roomIndex, int floorIndex, bool isLocked) // adds a new door into the vector 
{
	Door* d = new Door(roomIndex,floorIndex,side,&T_Door,isLocked,scale);   // creates a new door on heap, initialises it using the parameters
	d->setTiledPosition(referenceTile->getPosition());                      // uses the reference tile to set the position of the door (with the offset being added by the door)
	doors[floorIndex][roomIndex - 1].push_back(d);                          // pushes the door into the correct floor and room vector
}

void DoorManager::linkDoors(int D1_floorID, int D1_roomID, int D1_ID, int D2_floorID, int D2_roomID, int D2_ID)   // links th edoors using the parameters
{
	doors[D1_floorID][D1_roomID - 1][D1_ID]->setLinkedDoor(doors[D2_floorID][D2_roomID - 1][D2_ID]);   // sets the linked door using the parameters and sets the linked door's linked door to this 
}

void DoorManager::linkDoors(Door* D1, int D2_floorID, int D2_roomID, int D2_ID)  //method overload to set linked door in case we have the correct door already
{
	D1->setLinkedDoor(doors[D2_floorID][D2_roomID - 1][D2_ID]);    // sets the linked door  and linked door's to this
}

void DoorManager::update(float dt, sf::Vector2i currentLocation)
{
	for (int i = 0; i < getDoorsInRoom(currentLocation.x, currentLocation.y).size(); i++)      //iteartes through all the doors to call their update 
	{
		getDoorsInRoom(currentLocation.x, currentLocation.y)[i]->update(dt);            // calls their update, th edoors themselves have either empty or fuilled update but this call doesn't determine that
	}
}

void DoorManager::collision(Link* player)  // resolves the collision  with the player
{
	std::vector<Door*> current = doors[player->getCurrentLocation().x][player->getCurrentLocation().y - 1];  //gets the vector of pointers for the doors in the current room
	                       
	for (int i = 0; i < current.size(); i++) {                              //iterates through all the doors in the room 
		if (Collision::checkBoundingBox(player, current[i])) {              //checks for the collision with the player
			current[i]->collisionResponse(player);                          //if collision happens, transpports th eplayer using linked room atributes
		}
	}
}
