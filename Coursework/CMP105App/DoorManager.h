#pragma once
#include "Door.h"

class DoorManager
{
	std::vector<std::vector<std::vector<Door*>>> doors;      // a 3D array for doors, the storage structare is : [floor][room][doorID]
	sf::Texture T_Door;                                      // door texture that all the doors point to 
	int scale;                                               // scale for the texture

public:
	DoorManager(int numOfFloors, int numOfRooms, int scale); // constructor
	~DoorManager();

	void addDoor(GameObject* referenceTile, DoorSide side, int roomIndex, int floorIndex, bool isLocked);       //adds the door into the 3D vector while also initialising it
	void linkDoors(int D1_floorID, int D1_roomID, int D1_ID,int D2_floorID, int D2_roomID, int D2_ID);          //sets the linked door by using the location coordinates (floor,room,ID) of both door
	void linkDoors(Door* D1, int D2_floorID, int D2_roomID, int D2_ID);                                         //sets th elinked door using the door refference and door location coords
	void update(float dt, sf::Vector2i currentLocation);                                                        //calls the update function of the doors based the current floor and room
	void collision(Link* player);                                                                               //calls the collision response of the doors and player
	std::vector<Door*> getDoorsInRoom(int floorIndex, int roomIndex) {return doors[floorIndex][roomIndex-1];}   //returns the vector of pointers to the doors in the desired room
};

