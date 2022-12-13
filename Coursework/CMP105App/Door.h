#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"
#include "Link.h"

enum class DoorSide {NORTH,SOUTH,EAST,WEST};         // enumeration of all the sides for the door
enum class DoorType {BACK_TO_MENU, STANDARD , END};  // enumeration for the type of doors (used for menu return and credits screen calling)

class Door :
	public GameObject
{
	Door* linkedDoor;                                // door that this door is linked to (vice-versa)
	DoorSide side;                                   // the orientatio of the door
	DoorType type = DoorType::STANDARD;              // type of the door (standard unless specified)
	bool isLocked = false;                           // whether the door needs to be unlocked before transporting the player
	bool isKeyInserted = false;                      // for animation purposes,whether the key has been inserted but the door is yet to be unlocked
	int roomIndex;                                   // index of the room this door is in
	int floorIndex;                                  // index of the floor this door is in
	int scale;                                       // for texture size purposes
	float animationSpeed = 1.0f;                     // animation speed for unlocking the door
	sf::Texture* T_Door;                             // texture pointer to the door texture
	Animation A_doorUnlock;                          // animation for unlocking the door
	sf::Color C_transparent = sf::Color(0, 0, 0, 0); // transparent color
	sf::Color C_opaque = sf::Color(255,255,255,255); // opague color

public:

	Door(int roomIndex, int floorIndex, DoorSide side, sf::Texture* T_Door, bool isLocked, int scale);            //the constructor
	~Door();
	void collisionResponse(GameObject* collider) override;                                                        //collision with the player changes the position and location values for the player (resulting in a different room to be rendered)
	void setLinkedDoor(Door* linkedDoor) { this->linkedDoor = linkedDoor; this->linkedDoor->linkedDoor = this; }  //sets the linked door (usd for transporting the player) and sets this door to be the linked door of the other linked door
	int getRoomIndex() { return roomIndex; }                                                                      
	int getFloorIndex() { return floorIndex; }
	DoorSide getSide() { return side; }
	void setTiledPosition(sf::Vector2f basePosition);                                                             //sets the position of the door, uses the tileMap coordinates instead of just standard world coordinates
	bool unlock(Link* player);                                                                                    //tries to unlock the door 
	void lock();                                                                                                  //used only at th esetup stage where the door is set as locked using this method
	void update(float dt) override;                                                                               //only updates itself when playing the animation, otherwise does nothing
	void makeDoorType(DoorType type) { this->type = type; }                                                       //used only at the set up stage where the door tupe is set using this method 

};

