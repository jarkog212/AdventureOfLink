#include "Door.h"
// specific pixels for the texture
#define NORTH_SOUTH_CELL_X 48
#define NORTH_SOUTH_CELL_Y 53
#define EAST_WEST_CELL_X 53
#define EAST_WEST_CELL_Y 48

#define DOOR_TEXTURE_OFFSET_SIDE 24
#define DOOR_TEXTURE_OFFSET_TOP 11
#define TILE_SIZE 32


Door::Door(int roomIndex, int floorIndex, DoorSide side, sf::Texture* T_Door, bool isLocked, int scale)
{
	// initialises the pointers
	this->side = side;
	this->T_Door = T_Door;
	this->roomIndex = roomIndex;
	this->isLocked = isLocked;
	this->scale = scale;
	this->floorIndex = floorIndex;

	//switch based on the orientation
	switch (side) {
	case DoorSide::EAST:
		setSize(sf::Vector2f(EAST_WEST_CELL_X * scale, EAST_WEST_CELL_Y * scale));                                                      //sets the size base don the texture and scale
		A_doorUnlock.buildAnimation(sf::Vector2i(EAST_WEST_CELL_X, EAST_WEST_CELL_Y), NORTH_SOUTH_CELL_Y, 4, false, animationSpeed);    //builds the animation based on the side
		A_doorUnlock.setFlipped(true);                                                                                                  //needed for east/west 
		break;
	case DoorSide::WEST:
		setSize(sf::Vector2f(EAST_WEST_CELL_X * scale, EAST_WEST_CELL_Y * scale));
		A_doorUnlock.buildAnimation(sf::Vector2i(EAST_WEST_CELL_X, EAST_WEST_CELL_Y), NORTH_SOUTH_CELL_Y, 4, false, animationSpeed);
		break;
	case DoorSide::NORTH:
		setSize(sf::Vector2f(NORTH_SOUTH_CELL_X * scale, NORTH_SOUTH_CELL_Y * scale));
		A_doorUnlock.buildAnimation(sf::Vector2i(NORTH_SOUTH_CELL_X, NORTH_SOUTH_CELL_Y), 0, 4, false, animationSpeed);
		break;
	case DoorSide::SOUTH:
		setSize(sf::Vector2f(NORTH_SOUTH_CELL_X * scale, NORTH_SOUTH_CELL_Y * scale));
		A_doorUnlock.buildAnimation(sf::Vector2i(NORTH_SOUTH_CELL_X, NORTH_SOUTH_CELL_Y), NORTH_SOUTH_CELL_Y + EAST_WEST_CELL_Y, 4, false, animationSpeed);
		break;
	}
	if (isLocked) {                                     //if the door is locked, sets the texture by...
		setTexture(T_Door);								//...taking the texture pointer...
		A_doorUnlock.animate(0);                        //...setting the first frame of the animation...
		setTextureRect(A_doorUnlock.getCurrentFrame()); //...setting the first texture frame of the animation as the texture
	}
	else {
		setFillColor(C_transparent);                    //if its unlocked, there is no need for a texture to be shown
	}
	setCollider(true);                                            // the door is a collider in all the states
	setCollisionBox(sf::FloatRect(0,0,getSize().x,getSize().y));  // collision box matches the size of the texture
}

Door::~Door()
{
}

void Door::collisionResponse(GameObject* collider)      // only one type of collision response, uses the override method 
{
	Link* player = static_cast<Link*>(collider);        // since I know for afact that the only collider with the door is the player, I can cast the pointer to the player class statically
	
	//switch based on the type of the door
	switch (type) {      
	case DoorType::STANDARD:
		if (!isLocked) {                                                                               //only transports the player if the door is unlocked 
			player->setCurrentLocation(linkedDoor->getFloorIndex(), linkedDoor->getRoomIndex());       //change the players location values to the linked door values (by extension change the room that will be rendered)
			// switch based on the orientation room, used for proper player offset from the position of the door, moves the player to the linked doors location
			switch (linkedDoor->getSide()) {
			case DoorSide::SOUTH:
				player->setPosition(linkedDoor->getPosition() + sf::Vector2f((TILE_SIZE - DOOR_TEXTURE_OFFSET_SIDE) * scale, -(player->getSize().y + 1)));
				break;
			case DoorSide::WEST:
				player->setPosition(linkedDoor->getPosition() + sf::Vector2f(linkedDoor->getSize().x + 1, (TILE_SIZE - DOOR_TEXTURE_OFFSET_SIDE) * scale));
				break;
			case DoorSide::EAST:
				player->setPosition(linkedDoor->getPosition() + sf::Vector2f(-(player->getSize().x + 1), (TILE_SIZE - DOOR_TEXTURE_OFFSET_SIDE) * scale));
				break;
			case DoorSide::NORTH:
				player->setPosition(linkedDoor->getPosition() + sf::Vector2f((TILE_SIZE - DOOR_TEXTURE_OFFSET_SIDE) * scale, linkedDoor->getSize().x + 1));
				break;
			}
		}
		else {                                     // if the door is locked..
			player->wallColisionResponse(NULL);    //... it acts as a wall
		}
		break;
	case DoorType::BACK_TO_MENU:                           //in the case of the menu, sets the players health to -1.33, unachievable value by other means, which triggers the main menu screen...
		player->giveDamage(player->getHealth() + 1.33f);   //...this is done because neither the door nor the player have direct access to the gamestate object, level has so it can use the player health as a messanger
		break;
	case DoorType::END:                                    //in the case of credits, sets the player health to -1.11, unachievable value by other means. same logic for game state switching as above
		player->giveDamage(player->getHealth() + 1.11f);
		break;
	}
}

void Door::setTiledPosition(sf::Vector2f basePosition)     // sets the position of the based on the input + adds the texture offeset to better fit the tiled system 
{
	// uses the door orientation to move the object by proper offset and scale 
	switch (side) {
	case DoorSide::NORTH:
		setPosition(sf::Vector2f(basePosition.x + DOOR_TEXTURE_OFFSET_SIDE * scale, basePosition.y + DOOR_TEXTURE_OFFSET_TOP * scale));
		break;
	case DoorSide::SOUTH:
		setPosition(sf::Vector2f(basePosition.x + DOOR_TEXTURE_OFFSET_SIDE * scale, basePosition.y));
		break;
	case DoorSide::EAST:
		setPosition(sf::Vector2f(basePosition.x , basePosition.y + DOOR_TEXTURE_OFFSET_SIDE * scale));
		break;
	case DoorSide::WEST:
		setPosition(sf::Vector2f(basePosition.x + DOOR_TEXTURE_OFFSET_TOP * scale, basePosition.y + DOOR_TEXTURE_OFFSET_SIDE * scale));
		break;
	}
}

bool Door::unlock(Link* player)          //tries to unlock the door by using the keys, if the player has any 
{
	if (player->getDungeonKeys() > 0) {  //checks whether the player has any keys
		isKeyInserted = true;            //if yes, sets up for animation
		player->useDungeonKey();         //uses one of the keys
		return true;                     // returns true for successful door unlock
	}
	return false;                        //otherwise it wont do anything and returns false for the door unlock
}

void Door::lock()
{
	setFillColor(C_opaque);                            //when locked the door is visible...
	isLocked = true;                                   //...is locked...
	setTexture(T_Door);                                //...sets the texture...
	A_doorUnlock.animate(0);                           //...sets the first frame of the animation...
	setTextureRect(A_doorUnlock.getCurrentFrame());    //...sets the texture of the first frame
}

void Door::update(float dt)
{
	if (isKeyInserted && isLocked) {                          //if the key is inserted and the door is still locked, i.e. the unlock animation has not been finished... 
		if (A_doorUnlock.getPlaying() == true) {
			A_doorUnlock.animate(dt);                         //...continue the animation 
			setTextureRect(A_doorUnlock.getCurrentFrame());   //...set the texture from the current frame
		}
		else {                                                //if the animation is finished...
			isLocked = false;                                 //...set the door as unlocked...
			setTexture(NULL);                                 //..."deset" the texture... 
			setFillColor(C_transparent);                      //...set as transparent.
		}
	}
}


