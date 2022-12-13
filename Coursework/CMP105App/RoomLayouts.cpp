#include "RoomLayouts.h"

RoomLayouts::RoomLayouts(Input* input, AudioManager* audioManager)
{
	eventManager = EventManager(numOfFloors, numOfRooms, &tileSet, input, audioManager); //since it now has the audio manager and input pointers, event manager can be properly initialised
	enemyManager = EnemyManager(numOfFloors, numOfRooms, audioManager);                  //since it now has the audio manager pointer, enemy manager can be properly initialised

	std::vector<int> collisionMap                                                        //collision map for the tileset 0 = not a collider, 1 = collider. Matches the tileSet size
	{ 0,
	 0,0,0,0,0,0,0,0,0,0,1,1,1,0,
	 0,1,0,0,1,0,0,0,0,0,1,1,0,1,
	 0,0,0,1,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,1,1,1,0,0,0,
	 0,1,0,0,1,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,1,1,0,0,0,1,1,0,
	 1,1,1,1,0,0,1,0,0,0,0,0,0,1,
	 0,0,0,1,0,0,1,0,0,0,0,0,0,1,
	 0,0,0,1,0,0,1,0,0,0,0,0,0,0,
	 1,1,1,0,0,0,0,0,0,0,0,0,0,0,
	 1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	};
	tileSet.setCollision(collisionMap);                                                  //sets the collsion of the tiles in tileset from the defined vector 
	tileSet.setSemiCollider(153);                                                        //sets the tile at index 153 to alive (in tile context this means that ceratin things like arrows won't collide with it)
	createRoomsF1();                                                                     // calls for creation and initialisation of the rooms, separate to keep the cleanliness of the code

	for (int i = 0; i < rooms.size(); i++) {                                             //finalises the initialisation of the rooms by... 
		for (int j = 0; j < rooms[i].size(); j++) {
			for (int k = 0; k < rooms[i][j].size(); k++) {
				rooms[i][j][k].setPosition(sf::Vector2f(0, 0));                          //...setting their position...
				rooms[i][j][k].buildLevel();                                             //...building them
			}
		}
		setDoors(i);                                                                     //generates and sets the door 
		createEffects(i);                                                                //creates the effects
		createEvents(i);                                                                 //creates events
		createEnemies(i);                                                                //creates enemies
	}
}

RoomLayouts::~RoomLayouts()
{
	//properly deletes the 3D vector
	for (int i = 0; i < rooms.size(); i++) {
		for (int j = 0; j < rooms[i].size(); j++) {
			rooms[i][j].erase(rooms[i][j].begin(), rooms[i][j].end());  //erase all the room layers in room
		}
		rooms[i].erase(rooms[i].begin(), rooms[i].end());               //erase all the rooms on a floor
	}
	rooms.erase(rooms.begin(), rooms.end());                            //erase all the floors
}

void RoomLayouts::generateDoors(int floor) // uses the level tileMap to generate doors, generates them from top left to bottom right, hence its easy to predict the door index just by looking at the room
{
	std::vector<GameObject>* currentRoom;                            //stores current room to reduce access time
	std::vector<int> currentRoomTileMap;                             //stores the current room layout to reduce access time

	for (int i = 1; i <= rooms[floor].size(); i++) {                 //iterates throufgh the all the rooms on a given floor     
		currentRoomTileMap = rooms[floor][i - 1][0].getTileMap();    //saves the current rooms' tileMap vector (layer 0 as thats where the doors are defined) for reduced access time
		for (int j = 0; j < currentRoomTileMap.size(); j++) {        //iterates through the entire tileMap
			//switch based on the tile Index (determines the type as doors have specific indeces) and if door index matches, creates the door with proper initialisation
			switch (currentRoomTileMap[j]) {
			case 85:
				addDoor(i, floor, j, DoorSide::NORTH, false);
				break;
			case 88:
				addDoor(i, floor, j, DoorSide::WEST, false);
				break;
			case 90:
				addDoor(i, floor, j, DoorSide::EAST, false);
				break;
			case 113:
				addDoor(i, floor, j, DoorSide::SOUTH, false);
				break;
			}
		}
	}
}

void RoomLayouts::createEvents(int floor)
{
	// manually adds the events using the simplified code
	addEvent(EventType::CHEST, 0, 18, sf::Vector2i(18, 28));  // example: add a chest on floor 0, room 18, at tile 18,28 
	addEvent(EventType::CHEST, 0, 16, sf::Vector2i(7, 16));
	addEvent(EventType::CHEST, 0, 11, sf::Vector2i(2, 5));
	addEvent(EventType::CHEST, 0, 5, sf::Vector2i(25, 5));
	
	//in some cases further set up is necessary, which is allowed by the returned pointer and castin
	Event_KeyHole* tempKH;
	tempKH = static_cast<Event_KeyHole*>(addEvent(EventType::KEY_HOLE, 0, 4, sf::Vector2i(30, 4))); //example: add a keyhole on floor 0, room 4 at tile 30,4...
	tempKH->setLinkedDoor(doorManager.getDoorsInRoom(0,4)[0]);                                      //... and link it with the door 0 on floor 0, room 4

	tempKH = static_cast<Event_KeyHole*>(addEvent(EventType::KEY_HOLE, 0, 11, sf::Vector2i(18, 3)));
	tempKH->setLinkedDoor(doorManager.getDoorsInRoom(0, 11)[0]);

	tempKH = static_cast<Event_KeyHole*>(addEvent(EventType::KEY_HOLE, 0, 20, sf::Vector2i(8, 3)));
	tempKH->setLinkedDoor(doorManager.getDoorsInRoom(0, 20)[0]);

	tempKH = static_cast<Event_KeyHole*>(addEvent(EventType::KEY_HOLE, 0, 21, sf::Vector2i(15, 3)));
	tempKH->setLinkedDoor(doorManager.getDoorsInRoom(0, 21)[0]);

	Event_FlowSwitch* tempFS;
	tempFS = static_cast<Event_FlowSwitch*>(addEvent(EventType::FLOW_SWITCH, 0, 7, sf::Vector2i(14, 26)));                             //example: add a flow Switch on floor 0, room 7 at tile 14,26...
	tempFS->setLinkedRoom(&rooms[0][5-1][0],visualEffectManager.getEffectsInRoom(0,5),visualEffectManager.getEffectsInRoom(0,7));      //...set the linked room to be the room on floor 0, room 5. This will activate sand flowing in room on floor 0, room 5 and flowing out of the room on floor 0, room 7

	tempFS = static_cast<Event_FlowSwitch*>(addEvent(EventType::FLOW_SWITCH, 0, 13, sf::Vector2i(9, 8)));
	tempFS->setLinkedRoom(&rooms[0][18-1][0],visualEffectManager.getEffectsInRoom(0,18),visualEffectManager.getEffectsInRoom(0,13));

	tempFS = static_cast<Event_FlowSwitch*>(addEvent(EventType::FLOW_SWITCH, 0, 18, sf::Vector2i(2, 16)));
	tempFS->setLinkedRoom(&rooms[0][16 - 1][0], visualEffectManager.getEffectsInRoom(0, 16), visualEffectManager.getEffectsInRoom(0, 18));
}

void RoomLayouts::createEnemies(int floor)  
{
	//create the enemies manually using a simple function call
	//Room 2
	addEnemy(EnemyType::SPIKE, 0, 2, sf::Vector2i(4,7));      //example: create a Spike on floor 0, room 2 at tile 4,7 
	addEnemy(EnemyType::SPIKE, 0, 2, sf::Vector2i(4, 8));
	addEnemy(EnemyType::SPIKE, 0, 2, sf::Vector2i(5, 15));
	//Room 3
	addEnemy(EnemyType::GOOY, 0, 3, sf::Vector2i(8, 6));
	addEnemy(EnemyType::GOOY, 0, 3, sf::Vector2i(4, 6));
	addEnemy(EnemyType::GOOY, 0, 3, sf::Vector2i(12, 6));
	addEnemy(EnemyType::SPIKE, 0, 3, sf::Vector2i(5, 6));
	addEnemy(EnemyType::SPIKE, 0, 3, sf::Vector2i(6, 6));
	addEnemy(EnemyType::SPIKE, 0, 3, sf::Vector2i(5, 6));
	addEnemy(EnemyType::SPIKE, 0, 3, sf::Vector2i(11, 6));
	addEnemy(EnemyType::SPIKE, 0, 3, sf::Vector2i(10, 6));
	//Room 4
	addEnemy(EnemyType::SKELETOR, 0, 4, sf::Vector2i(11, 15));
	addEnemy(EnemyType::SANDY, 0, 4, sf::Vector2i(28, 15));
	addEnemy(EnemyType::GOOY, 0, 4, sf::Vector2i(30, 5));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(27, 6));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(28, 6));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(29, 6));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(30, 6));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(32, 9));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(31, 9));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(30, 9));
	addEnemy(EnemyType::SPIKE, 0, 4, sf::Vector2i(29, 9));
	//Room 5
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(8, 7));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(11, 7));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(14, 7));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(8, 10));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(14, 10));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(8, 13));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(11, 13));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(14, 13));
	addEnemy(EnemyType::SKELETOR, 0, 5, sf::Vector2i(8, 4));
	addEnemy(EnemyType::SKELETOR, 0, 5, sf::Vector2i(11, 4));
	addEnemy(EnemyType::SKELETOR, 0, 5, sf::Vector2i(14, 4));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(3, 6));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(25, 8));
	addEnemy(EnemyType::SPIKE, 0, 5, sf::Vector2i(26, 6));
	//Room 6 
	addEnemy(EnemyType::SPIKE, 0, 6, sf::Vector2i(5, 5));
	addEnemy(EnemyType::SPIKE, 0, 6, sf::Vector2i(6, 5));
	addEnemy(EnemyType::SPIKE, 0, 6, sf::Vector2i(5, 6));
	addEnemy(EnemyType::SPIKE, 0, 6, sf::Vector2i(6, 6));
	addEnemy(EnemyType::GOOBY, 0, 6, sf::Vector2i(3, 4));
	//Room 7 
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(12, 8));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(12, 10));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(12, 12));
	addEnemy(EnemyType::SANDY, 0, 7, sf::Vector2i(11, 14));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(18, 9));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(18, 11));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(18, 13));
	addEnemy(EnemyType::SKELETOR, 0, 7, sf::Vector2i(17, 7));
	addEnemy(EnemyType::SKELETOR, 0, 7, sf::Vector2i(18, 7));
	addEnemy(EnemyType::GOOY, 0, 7, sf::Vector2i(10, 11));
	addEnemy(EnemyType::GOOY, 0, 7, sf::Vector2i(20, 8));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(8, 24));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(9, 24));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(8, 25));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(9, 25));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(13, 28));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(14, 28));
	addEnemy(EnemyType::SPIKE, 0, 7, sf::Vector2i(15, 28));
	addEnemy(EnemyType::SKELETOR, 0, 7, sf::Vector2i(23, 26));
	addEnemy(EnemyType::SANDY, 0, 7, sf::Vector2i(6, 22));
	addEnemy(EnemyType::GOOY, 0, 7, sf::Vector2i(6, 29));
	addEnemy(EnemyType::GOOY, 0, 7, sf::Vector2i(19, 30));
	//Room 8
	addEnemy(EnemyType::SPIKE, 0, 8, sf::Vector2i(5, 7));
	addEnemy(EnemyType::SPIKE, 0, 8, sf::Vector2i(6, 7));
	addEnemy(EnemyType::SPIKE, 0, 8, sf::Vector2i(7, 7));
	addEnemy(EnemyType::SKELETOR, 0, 8, sf::Vector2i(2, 5));
	addEnemy(EnemyType::SKELETOR, 0, 8, sf::Vector2i(10, 5));
	//Room 9
	addEnemy(EnemyType::SKELETOR, 0, 9, sf::Vector2i(9, 9));
	addEnemy(EnemyType::GOOY, 0, 9, sf::Vector2i(15, 7));
	addEnemy(EnemyType::GOOY, 0, 9, sf::Vector2i(15, 10));
	addEnemy(EnemyType::GOOBY, 0, 9, sf::Vector2i(9, 3));
	addEnemy(EnemyType::GOOBY, 0, 9, sf::Vector2i(3, 7));
	//Room 10
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(27, 6));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(27, 7));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(27, 8));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(28, 6));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(28, 7));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(28, 8));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(4, 6));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(4, 7));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(4, 8));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(5, 6));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(5, 7));
	addEnemy(EnemyType::SPIKE, 0, 10, sf::Vector2i(5, 8));
	addEnemy(EnemyType::SANDY, 0, 10, sf::Vector2i(11, 6));
	addEnemy(EnemyType::SANDY, 0, 10, sf::Vector2i(15, 6));
	addEnemy(EnemyType::SANDY, 0, 10, sf::Vector2i(20, 6));
	addEnemy(EnemyType::GOOY, 0, 10, sf::Vector2i(6, 6));
	addEnemy(EnemyType::GOOY, 0, 10, sf::Vector2i(6, 7));
	addEnemy(EnemyType::GOOY, 0, 10, sf::Vector2i(6, 8));
	addEnemy(EnemyType::SKELETOR, 0, 10, sf::Vector2i(27, 3));
	//Room 11
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(20, 25));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(20, 27));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(10, 25));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(10, 27));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(15, 30));
	addEnemy(EnemyType::GOOY, 0, 11, sf::Vector2i(18, 26));
	addEnemy(EnemyType::GOOY, 0, 11, sf::Vector2i(15, 29));
	addEnemy(EnemyType::GOOY, 0, 11, sf::Vector2i(12, 26));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(15, 16));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(15, 17));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(13, 15));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(14, 15));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(13, 7));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(14, 7));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(16, 11));
	addEnemy(EnemyType::SPIKE, 0, 11, sf::Vector2i(17, 11));
	addEnemy(EnemyType::SANDY, 0, 11, sf::Vector2i(4, 17));
	addEnemy(EnemyType::SANDY, 0, 11, sf::Vector2i(5, 17));
	addEnemy(EnemyType::GOOY, 0, 11, sf::Vector2i(7, 9));
	addEnemy(EnemyType::GOOBY, 0, 11, sf::Vector2i(2, 3));
	addEnemy(EnemyType::SANDY, 0, 11, sf::Vector2i(25, 16));
	addEnemy(EnemyType::SANDY, 0, 11, sf::Vector2i(24, 16));
	addEnemy(EnemyType::SKELETOR, 0, 11, sf::Vector2i(24, 6));
	addEnemy(EnemyType::SKELETOR, 0, 11, sf::Vector2i(27, 6));
	//Room 12
	addEnemy(EnemyType::GOOY, 0, 12, sf::Vector2i(12, 7));
	addEnemy(EnemyType::GOOY, 0, 12, sf::Vector2i(18, 7));
	addEnemy(EnemyType::GOOY, 0, 12, sf::Vector2i(15, 7));
	addEnemy(EnemyType::SANDY, 0, 12, sf::Vector2i(15, 16));
	addEnemy(EnemyType::SPIKE, 0, 12, sf::Vector2i(12, 17));
	addEnemy(EnemyType::SPIKE, 0, 12, sf::Vector2i(18, 17));
	addEnemy(EnemyType::SANDY, 0, 12, sf::Vector2i(8, 23));
	addEnemy(EnemyType::GOOY, 0, 12, sf::Vector2i(19, 23));
	addEnemy(EnemyType::GOOY, 0, 12, sf::Vector2i(21, 21));
	addEnemy(EnemyType::GOOBY, 0, 12, sf::Vector2i(25, 20));
	addEnemy(EnemyType::SPIKE, 0, 12, sf::Vector2i(12, 22));
	addEnemy(EnemyType::SPIKE, 0, 12, sf::Vector2i(13, 22));
	addEnemy(EnemyType::SPIKE, 0, 12, sf::Vector2i(12, 23));
	addEnemy(EnemyType::SPIKE, 0, 12, sf::Vector2i(13, 23));
	//Room 13
	addEnemy(EnemyType::SPIKE, 0, 13, sf::Vector2i(13, 11));
	addEnemy(EnemyType::SPIKE, 0, 13, sf::Vector2i(13, 12));
	addEnemy(EnemyType::SPIKE, 0, 13, sf::Vector2i(13, 15));
	addEnemy(EnemyType::SPIKE, 0, 13, sf::Vector2i(13, 16));
	addEnemy(EnemyType::SPIKE, 0, 13, sf::Vector2i(13, 17));
	addEnemy(EnemyType::SKELETOR, 0, 13, sf::Vector2i(23, 6));
	addEnemy(EnemyType::GOOY, 0, 13, sf::Vector2i(16, 8));
	addEnemy(EnemyType::GOOY, 0, 13, sf::Vector2i(23, 12));
	//Room 14
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(13, 3));
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(14, 3));
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(13, 4));
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(14, 4));
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(4, 5));
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(5, 5));
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(4, 6));
	addEnemy(EnemyType::SPIKE, 0, 14, sf::Vector2i(5, 6));
	addEnemy(EnemyType::SANDY, 0, 14, sf::Vector2i(7, 3));
	addEnemy(EnemyType::SANDY, 0, 14, sf::Vector2i(5, 15));
	//Room 15
	addEnemy(EnemyType::SPIKE, 0, 15, sf::Vector2i(13, 5));
	addEnemy(EnemyType::SPIKE, 0, 15, sf::Vector2i(14, 5));
	addEnemy(EnemyType::GOOY, 0, 15, sf::Vector2i(3, 5));
	addEnemy(EnemyType::GOOY, 0, 15, sf::Vector2i(6, 5));
	//Room 16
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(27, 9));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(28, 9));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(27, 10));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(28, 10));
	addEnemy(EnemyType::SANDY, 0, 16, sf::Vector2i(22, 7));
	addEnemy(EnemyType::SKELETOR, 0, 16, sf::Vector2i(22, 13));
	addEnemy(EnemyType::GOOY, 0, 16, sf::Vector2i(28, 5));
	addEnemy(EnemyType::GOOY, 0, 16, sf::Vector2i(28, 12));
	addEnemy(EnemyType::GOOY, 0, 16, sf::Vector2i(21, 10));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(10, 16));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(9, 15));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(8, 14));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(8, 13));
	addEnemy(EnemyType::SPIKE, 0, 16, sf::Vector2i(7, 12));
	addEnemy(EnemyType::SANDY, 0, 16, sf::Vector2i(5, 6));
	addEnemy(EnemyType::SKELETOR, 0, 16, sf::Vector2i(4, 17));
	addEnemy(EnemyType::GOOY, 0, 16, sf::Vector2i(7, 14));
	//Room 17
	addEnemy(EnemyType::GOOY, 0, 17, sf::Vector2i(8, 8));
	addEnemy(EnemyType::GOOY, 0, 17, sf::Vector2i(3, 14));
	addEnemy(EnemyType::GOOY, 0, 17, sf::Vector2i(9, 15));
	//Room 18
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(16, 4));
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(24, 4));
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(16, 14));
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(24, 14));
	addEnemy(EnemyType::GOOBY, 0, 18, sf::Vector2i(16, 9));
	addEnemy(EnemyType::GOOBY, 0, 18, sf::Vector2i(24, 9));
	addEnemy(EnemyType::SKELETOR, 0, 18, sf::Vector2i(21, 26));
	addEnemy(EnemyType::SKELETOR, 0, 18, sf::Vector2i(20, 29));
	addEnemy(EnemyType::SANDY, 0, 18, sf::Vector2i(3, 28));
	addEnemy(EnemyType::GOOY, 0, 18, sf::Vector2i(2, 20));
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(6, 16));
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(6, 18));
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(6, 20));
	addEnemy(EnemyType::SPIKE, 0, 18, sf::Vector2i(6, 22));
	//Room 19
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(5,4));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(7, 4));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(9, 4));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(11, 4));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(13, 4));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(5, 7));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(7, 7));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(9, 7));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(11, 7));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(13, 7));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(21, 6));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(22, 6));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(25, 6));
	addEnemy(EnemyType::SPIKE, 0, 19, sf::Vector2i(26, 6));
	addEnemy(EnemyType::GOOY, 0, 19, sf::Vector2i(5, 5));
	addEnemy(EnemyType::GOOY, 0, 19, sf::Vector2i(10, 5));
	addEnemy(EnemyType::GOOY, 0, 19, sf::Vector2i(19, 4));
	addEnemy(EnemyType::SKELETOR, 0, 19, sf::Vector2i(22, 5));
	//Room 20
	addEnemy(EnemyType::SKELETOR, 0, 20, sf::Vector2i(1, 6));
	addEnemy(EnemyType::SKELETOR, 0, 20, sf::Vector2i(2, 6));
	addEnemy(EnemyType::SKELETOR, 0, 20, sf::Vector2i(8, 6));
	addEnemy(EnemyType::SKELETOR, 0, 20, sf::Vector2i(9, 6));
	//Room 21
	addEnemy(EnemyType::SPIKE, 0, 21, sf::Vector2i(10, 3));
	addEnemy(EnemyType::SPIKE, 0, 21, sf::Vector2i(10, 4));
	addEnemy(EnemyType::SPIKE, 0, 21, sf::Vector2i(10, 5));
	addEnemy(EnemyType::SPIKE, 0, 21, sf::Vector2i(16, 3));
	addEnemy(EnemyType::SPIKE, 0, 21, sf::Vector2i(16, 4));
	addEnemy(EnemyType::SPIKE, 0, 21, sf::Vector2i(16, 5));
	addEnemy(EnemyType::SKELETOR, 0, 21, sf::Vector2i(8, 11));
	addEnemy(EnemyType::SKELETOR, 0, 21, sf::Vector2i(5, 8));
	addEnemy(EnemyType::SKELETOR, 0, 21, sf::Vector2i(5, 4));
	addEnemy(EnemyType::SKELETOR, 0, 21, sf::Vector2i(18, 11));
	addEnemy(EnemyType::SKELETOR, 0, 21, sf::Vector2i(21, 8));
	addEnemy(EnemyType::SKELETOR, 0, 21, sf::Vector2i(21, 4));
	addEnemy(EnemyType::GOOBY, 0, 21, sf::Vector2i(3, 4));
	addEnemy(EnemyType::GOOBY, 0, 21, sf::Vector2i(23, 4));
}

void RoomLayouts::createEffects(int floor)
{
	//create the effects manually using a simple function call
	//Room 5
	addEffect(VE_Type::STREAM_IN, DirectionState::DOWN, 0, 5, sf::Vector2i(20, 1));   //example: create the sand stream in flowing donwards on floor 0, room 5  at tile 20,1
	addEffect(VE_Type::SANDFALL, DirectionState::DOWN, 0, 5, sf::Vector2i(20, 2));
	addEffect(VE_Type::SANDFALL, DirectionState::DOWN, 0, 5, sf::Vector2i(21, 2));
	//Room 13
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(0, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(2, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(4, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(6, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(8, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(10, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(12, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(14, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(16, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(18, 13));
	addEffect(VE_Type::STREAM_OUT, DirectionState::LEFT, 0, 13, sf::Vector2i(20, 13));
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 13, sf::Vector2i(20, 8));
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 13, sf::Vector2i(20, 6));
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 13, sf::Vector2i(20, 4));
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 13, sf::Vector2i(20, 2));
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 13, sf::Vector2i(20, 1));
	//Room 14
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 14, sf::Vector2i(15, 18));
	addEffect(VE_Type::SANDFALL, DirectionState::UP, 0, 14, sf::Vector2i(15, 18));
	addEffect(VE_Type::SANDFALL, DirectionState::UP, 0, 14, sf::Vector2i(16, 18));
	//Room 16
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 16, sf::Vector2i(15, 23));
	addEffect(VE_Type::STREAM_IN, DirectionState::UP, 0, 16, sf::Vector2i(15, 21));
	addEffect(VE_Type::SANDFALL, DirectionState::UP, 0, 16, sf::Vector2i(15, 21));
	addEffect(VE_Type::SANDFALL, DirectionState::UP, 0, 16, sf::Vector2i(16, 21));
	//Room 18
	addEffect(VE_Type::STREAM_OUT, DirectionState::UP, 0, 18, sf::Vector2i(4, 21));
	addEffect(VE_Type::STREAM_OUT, DirectionState::UP, 0, 18, sf::Vector2i(4, 19));
	addEffect(VE_Type::STREAM_OUT, DirectionState::UP, 0, 18, sf::Vector2i(4, 17));
	addEffect(VE_Type::STREAM_OUT, DirectionState::UP, 0, 18, sf::Vector2i(4, 15));
	addEffect(VE_Type::STREAM_OUT, DirectionState::UP, 0, 18, sf::Vector2i(4, 14));
	addEffect(VE_Type::STREAM_IN, DirectionState::LEFT, 0, 18, sf::Vector2i(26, 19));
	addEffect(VE_Type::SANDFALL, DirectionState::LEFT, 0, 18, sf::Vector2i(26, 19));
	addEffect(VE_Type::SANDFALL, DirectionState::LEFT, 0, 18, sf::Vector2i(26, 20));
	

}

void RoomLayouts::createRoomsF1()
{
	//needed for proper vector functioning
	std::vector<std::vector<RoomMap>> d;
	rooms.push_back(d);
	
	//containers for layers until properly stored into the 3d vector
	std::vector<RoomMap> roomsMaps_F1_L1;
	std::vector<RoomMap> roomsMaps_F1_L2;
	std::vector<RoomMap> roomsMaps_F1_L3;

	//create each room manually, by adding maps into the simple function that will save them in the proper container 
	//Room 1
	std::vector<int>* map = new std::vector<int>{                   
		  1,   2,   3,   4,  85,  86,  87,   4,   3,   5,   6,
		 15,  16, 141, 142,  99, 100, 101, 142, 141,  19,  20,
		 43, 147,  31,  31,  31,  79,  80,  80,  80, 143,  48,
		 27,  28,  31,  31,  31,  79,  80,  80,  80,  13,  14,
		 27,  28,  66,  67,  31,  93,  94,  94,  94,  13,  14,
		 43, 147,  80,  81,  31,  31,  31,  31,  31, 143,  48,
		 57,  58, 145, 146, 113, 114, 115, 146, 145,  61,  62,
		 71,  72,  73,  74, 127, 128, 129,  74,  73,  75,  76, };
	setRoom(map, sf::Vector2u(11, 8), &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>;
	setRoom(map, sf::Vector2u(0, 0), &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>;
	setRoom(map, sf::Vector2u(0, 0), &roomsMaps_F1_L3, &tileSet);

	//Room 2
	sf::Vector2u mapDimensions = sf::Vector2u(11, 20);          //example: set the map dimension in tiles...
	map = new std::vector<int>{                                 //...define the layer 1 map as a vector (imported array from 3d party software)...
		1, 2, 3, 11, 11, 11, 11, 11, 3, 5, 6, 15, 16, 17, 25, 25, 25, 25, 25, 17, 19, 20, 43, 44, 1, 2, 85, 86, 87, 5, 6, 47, 48, 43, 44, 15, 16, 99, 100, 101, 19, 20, 47, 48, 43, 44, 43, 147, 79, 80, 80, 143, 48, 47, 48, 29, 30, 29, 148, 79, 80, 80, 144, 34, 33, 34, 43, 44, 43, 147, 79, 80, 80, 143, 48, 33, 34, 29, 30, 43, 147, 93, 80, 80, 144, 34, 33, 34, 43, 44, 29, 148, 31, 93, 94, 144, 34, 47, 48, 43, 44, 43, 147, 31, 31, 31, 143, 48, 47, 48, 43, 44, 57, 58, 108, 108, 108, 61, 62, 47, 48, 43, 44, 71, 145, 108, 108, 108, 145, 76, 47, 48, 43, 44, 31, 31, 31, 31, 79, 80, 80, 47, 48, 43, 147, 31, 31, 31, 31, 79, 80, 80, 143, 48, 43, 147, 66, 66, 66, 66, 80, 80, 80, 143, 48, 43, 147, 80, 80, 80, 80, 80, 80, 80, 143, 48, 43, 147, 80, 80, 80, 80, 80, 80, 80, 143, 48, 43, 44, 80, 80, 80, 80, 80, 80, 80, 47, 48, 57, 58, 59, 145, 113, 114, 115, 145, 59, 61, 62, 71, 72, 73, 73, 127, 128, 129, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);    //... add the map with the dimensions using the tileset into the container for layer 1...

	map = new std::vector<int>{                                 //... define the layer 3 map as a vector...   
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);    //... add the map with the dimensions using the tileset into the container for layer 2...

	map = new std::vector<int>{                                 //... define the layer 3 map as a vector...   
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);    //... add the map with the dimensions using the tileset into the container for layer 3

	//Room 3
	mapDimensions = sf::Vector2u(17, 13);
	map = new std::vector<int>{
		1, 2, 4, 3, 4, 4, 3, 11, 11, 11, 3, 4, 4, 3, 4, 5, 6, 15, 16, 142, 141, 18, 18, 141, 142, 25, 142, 141, 18, 18, 141, 142, 19, 20, 43, 147, 79, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 143, 48, 88, 89, 79, 80, 80, 80, 94, 94, 94, 94, 94, 94, 94, 94, 94, 90, 91, 102, 103, 93, 94, 94, 95, 31, 31, 65, 66, 66, 67, 31, 31, 31, 104, 105, 116, 117, 66, 66, 66, 66, 67, 31, 79, 80, 80, 81, 31, 31, 31, 118, 119, 43, 147, 80, 80, 80, 80, 80, 67, 93, 80, 80, 81, 31, 31, 31, 143, 48, 43, 147, 80, 80, 80, 80, 80, 81, 31, 79, 80, 80, 67, 31, 31, 143, 48, 27, 28, 80, 80, 80, 80, 80, 80, 67, 79, 80, 80, 81, 31, 31, 33, 34, 27, 28, 80, 80, 80, 80, 80, 80, 81, 93, 80, 80, 81, 31, 31, 143, 34, 43, 147, 80, 80, 80, 80, 80, 80, 81, 31, 79, 80, 81, 31, 31, 143, 48, 57, 58, 145, 146, 145, 146, 145, 113, 114, 115, 145, 146, 145, 146, 145, 61, 62, 71, 72, 73, 74, 73, 74, 73, 127, 128, 129, 73, 74, 73, 74, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 52, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 38, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 4
	mapDimensions = sf::Vector2u(35, 20);
	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 85, 86, 87, 3, 3, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 17, 99, 100, 101, 17, 141, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 44, 31, 65, 80, 80, 80, 80, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 79, 80, 80, 80, 80, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 79, 80, 80, 80, 80, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 93, 80, 80, 80, 94, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 31, 93, 94, 95, 31, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 31, 31, 65, 66, 66, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 66, 66, 67, 79, 80, 80, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 80, 80, 81, 93, 80, 80, 143, 48, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 122, 147, 80, 80, 81, 31, 93, 94, 143, 48, 15, 16, 141, 141, 141, 141, 17, 17, 17, 17, 17, 141, 141, 141, 141, 17, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 150, 94, 94, 95, 31, 31, 31, 143, 48, 43, 147, 66, 67, 31, 21, 153, 22, 22, 22, 153, 23, 79, 80, 81, 16, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 141, 92, 92, 92, 92, 92, 19, 47, 48, 88, 89, 80, 81, 31, 21, 153, 153, 153, 153, 153, 23, 79, 80, 81, 107, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 13, 14, 48, 102, 103, 80, 80, 67, 35, 36, 36, 36, 36, 36, 37, 93, 94, 95, 107, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 13, 14, 48, 116, 117, 80, 80, 81, 7, 8, 8, 8, 8, 8, 9, 31, 31, 31, 107, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 13, 14, 48, 43, 147, 80, 80, 81, 21, 153, 22, 22, 22, 153, 23, 31, 31, 31, 107, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 13, 14, 48, 43, 147, 80, 80, 95, 21, 153, 22, 22, 22, 153, 23, 31, 31, 31, 58, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 61, 47, 48, 57, 58, 145, 145, 145, 145, 59, 59, 59, 59, 59, 145, 145, 145, 145, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 59, 61, 62, 71, 72, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 5
	mapDimensions = sf::Vector2u(31, 20);
	map = new std::vector<int>{
		1, 2, 3, 11, 11, 85, 86, 87, 11, 3, 3, 11, 11, 11, 3, 3, 3, 4, 4, 137, 64, 64, 134, 4, 4, 3, 3, 3, 3, 5, 6, 15, 16, 17, 25, 25, 99, 100, 101, 25, 17, 141, 25, 25, 25, 141, 17, 141, 142, 142, 141, 17, 17, 141, 142, 142, 141, 141, 141, 141, 19, 20, 43, 44, 80, 94, 94, 95, 31, 93, 94, 94, 94, 94, 94, 94, 94, 94, 94, 95, 21, 153, 22, 22, 153, 23, 79, 80, 81, 31, 31, 143, 48, 27, 28, 95, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 21, 153, 22, 22, 153, 23, 79, 80, 81, 31, 31, 143, 48, 27, 28, 8, 8, 8, 8, 9, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 21, 153, 22, 22, 153, 23, 79, 80, 80, 67, 31, 146, 34, 43, 147, 153, 153, 153, 22, 23, 65, 66, 67, 65, 66, 67, 65, 66, 67, 31, 31, 21, 153, 22, 22, 153, 23, 79, 80, 80, 80, 67, 143, 48, 88, 89, 36, 36, 21, 22, 23, 79, 80, 81, 79, 80, 81, 79, 80, 81, 31, 31, 21, 153, 22, 22, 153, 23, 79, 80, 80, 80, 80, 146, 34, 102, 103, 31, 31, 21, 153, 23, 93, 94, 95, 93, 94, 95, 93, 94, 95, 31, 31, 21, 153, 22, 22, 153, 23, 93, 94, 80, 80, 80, 146, 34, 116, 117, 31, 31, 21, 153, 23, 65, 66, 67, 65, 66, 67, 65, 66, 67, 31, 31, 21, 153, 22, 22, 153, 23, 31, 31, 79, 80, 80, 143, 48, 43, 147, 8, 8, 21, 22, 23, 79, 80, 81, 79, 80, 81, 79, 80, 81, 31, 31, 21, 153, 22, 22, 153, 23, 31, 31, 93, 94, 80, 143, 48, 43, 44, 22, 153, 22, 153, 23, 93, 94, 95, 93, 94, 95, 93, 94, 95, 31, 31, 35, 152, 145, 146, 146, 145, 145, 146, 146, 146, 145, 61, 62, 43, 147, 153, 22, 153, 153, 23, 65, 66, 67, 65, 66, 67, 65, 66, 67, 31, 31, 31, 143, 112, 74, 74, 73, 73, 74, 74, 74, 73, 75, 76, 43, 147, 36, 36, 21, 22, 23, 79, 80, 81, 79, 80, 81, 79, 80, 81, 31, 31, 31, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 67, 31, 21, 153, 23, 93, 94, 95, 93, 94, 95, 93, 94, 95, 31, 31, 31, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 81, 31, 21, 153, 23, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 81, 31, 21, 22, 23, 21, 22, 153, 22, 153, 22, 153, 22, 153, 22, 153, 22, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 81, 31, 35, 36, 37, 35, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 80, 67, 31, 65, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 67, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 57, 58, 145, 113, 114, 115, 12, 145, 12, 145, 145, 12, 12, 12, 145, 145, 12, 145, 12, 61, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 72, 73, 127, 128, 129, 26, 73, 26, 73, 73, 26, 26, 26, 73, 73, 26, 73, 26, 75, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 38, 0, 38, 0, 38, 0, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 6
	mapDimensions = sf::Vector2u(12, 11);
	map = new std::vector<int>{
		1, 2, 4, 4, 11, 11, 11, 11, 4, 4, 5, 6, 15, 16, 18, 18, 25, 25, 25, 25, 18, 18, 19, 20, 29, 30, 79, 80, 80, 80, 80, 80, 80, 67, 33, 34, 29, 30, 79, 80, 80, 94, 94, 80, 80, 81, 33, 34, 88, 89, 79, 80, 81, 31, 31, 79, 80, 81, 90, 91, 102, 103, 79, 80, 81, 31, 31, 79, 80, 81, 104, 105, 116, 117, 93, 80, 80, 66, 66, 80, 80, 81, 118, 119, 29, 30, 31, 93, 80, 80, 80, 80, 80, 95, 33, 34, 29, 30, 31, 31, 93, 94, 94, 94, 95, 31, 33, 34, 57, 58, 60, 60, 12, 12, 12, 12, 60, 60, 61, 62, 71, 72, 74, 74, 26, 26, 26, 26, 74, 74, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 7
	mapDimensions = sf::Vector2u(26, 32);
	map = new std::vector<int>{
		0, 0, 0, 0, 0, 1, 2, 3, 3, 4, 3, 11, 11, 11, 3, 4, 3, 4, 3, 11, 11, 11, 3, 3, 5, 6, 0, 0, 0, 0, 0, 15, 16, 17, 17, 18, 17, 25, 25, 25, 17, 18, 17, 18, 17, 25, 25, 25, 17, 17, 19, 20, 0, 0, 0, 0, 0, 43, 44, 1, 2, 4, 3, 11, 11, 11, 3, 4, 3, 4, 3, 11, 11, 11, 5, 6, 47, 48, 0, 0, 0, 0, 0, 43, 44, 15, 16, 18, 141, 25, 25, 25, 141, 18, 141, 142, 141, 25, 25, 25, 19, 20, 47, 48, 0, 0, 0, 0, 0, 43, 44, 43, 44, 22, 23, 79, 80, 81, 21, 22, 23, 31, 31, 79, 81, 31, 143, 48, 47, 48, 0, 0, 0, 0, 0, 43, 44, 29, 30, 153, 23, 79, 80, 81, 21, 153, 23, 31, 65, 80, 81, 31, 143, 34, 47, 48, 0, 0, 0, 0, 0, 29, 30, 27, 28, 22, 23, 79, 80, 81, 35, 36, 37, 65, 80, 80, 80, 66, 13, 14, 33, 34, 0, 0, 0, 0, 0, 29, 30, 27, 28, 36, 37, 79, 80, 80, 66, 66, 66, 80, 80, 80, 80, 94, 13, 14, 33, 34, 0, 0, 0, 0, 0, 43, 44, 29, 148, 66, 67, 93, 80, 80, 80, 94, 94, 80, 80, 94, 95, 31, 143, 34, 47, 48, 0, 0, 0, 0, 0, 29, 30, 43, 147, 94, 80, 67, 93, 80, 81, 31, 31, 79, 81, 31, 31, 31, 90, 91, 47, 48, 0, 0, 0, 0, 0, 43, 44, 29, 30, 31, 79, 81, 31, 93, 81, 31, 31, 79, 80, 67, 31, 31, 104, 105, 47, 48, 0, 0, 0, 0, 0, 29, 30, 43, 147, 66, 80, 81, 31, 31, 93, 66, 66, 80, 80, 81, 31, 31, 118, 119, 47, 48, 0, 0, 0, 0, 0, 43, 44, 29, 148, 94, 94, 95, 31, 31, 31, 93, 80, 80, 80, 95, 31, 31, 143, 34, 47, 48, 0, 0, 0, 0, 0, 29, 30, 27, 28, 31, 31, 65, 66, 66, 67, 31, 93, 94, 95, 7, 8, 8, 13, 14, 33, 34, 0, 0, 0, 0, 0, 29, 30, 27, 28, 31, 65, 80, 80, 80, 81, 31, 31, 31, 31, 21, 153, 153, 13, 14, 33, 34, 0, 0, 0, 0, 0, 43, 44, 29, 148, 65, 80, 94, 94, 94, 81, 31, 31, 31, 31, 21, 153, 22, 33, 34, 47, 48, 0, 0, 0, 0, 0, 43, 44, 43, 147, 80, 81, 7, 8, 9, 79, 66, 66, 66, 67, 21, 153, 22, 47, 48, 47, 48, 0, 0, 0, 0, 0, 43, 44, 43, 147, 80, 81, 21, 22, 23, 79, 80, 80, 80, 81, 21, 153, 22, 47, 48, 47, 48, 1, 2, 60, 11, 60, 122, 123, 57, 58, 146, 146, 146, 146, 146, 146, 108, 108, 108, 146, 146, 60, 60, 61, 62, 47, 48, 15, 16, 74, 25, 74, 136, 137, 58, 145, 145, 145, 145, 145, 145, 145, 108, 108, 108, 145, 145, 145, 145, 145, 61, 47, 48, 43, 44, 153, 153, 153, 23, 79, 80, 81, 31, 31, 79, 80, 81, 31, 31, 31, 31, 79, 80, 80, 94, 94, 94, 143, 48, 43, 147, 36, 36, 36, 37, 79, 80, 80, 66, 66, 80, 80, 95, 31, 31, 31, 31, 79, 80, 81, 31, 31, 31, 47, 48, 43, 44, 66, 66, 66, 66, 80, 80, 94, 94, 80, 80, 95, 31, 31, 31, 31, 31, 93, 80, 81, 31, 31, 31, 143, 48, 88, 89, 94, 94, 94, 80, 80, 81, 31, 31, 79, 80, 66, 66, 66, 66, 66, 67, 31, 79, 80, 66, 66, 66, 13, 14, 102, 103, 31, 31, 31, 93, 80, 81, 31, 31, 79, 80, 80, 94, 94, 94, 80, 81, 31, 93, 94, 94, 80, 80, 13, 14, 116, 117, 8, 8, 8, 9, 79, 80, 66, 66, 80, 80, 81, 31, 31, 31, 79, 81, 31, 65, 66, 66, 80, 80, 13, 14, 29, 30, 153, 153, 153, 23, 79, 80, 80, 80, 80, 94, 80, 66, 66, 66, 80, 81, 31, 79, 80, 94, 94, 94, 13, 14, 29, 30, 22, 22, 153, 23, 93, 94, 94, 94, 95, 31, 93, 94, 80, 80, 80, 81, 31, 79, 81, 7, 8, 8, 143, 48, 29, 30, 22, 22, 153, 23, 7, 8, 8, 8, 8, 9, 65, 66, 80, 80, 80, 95, 65, 80, 81, 21, 153, 153, 33, 34, 29, 30, 22, 22, 153, 23, 21, 153, 22, 22, 153, 23, 79, 80, 80, 80, 80, 66, 80, 80, 81, 21, 153, 22, 33, 34, 57, 58, 60, 60, 60, 145, 145, 60, 59, 59, 60, 145, 145, 12, 145, 12, 145, 12, 145, 12, 145, 145, 60, 60, 61, 62, 71, 72, 74, 74, 74, 73, 73, 74, 73, 73, 74, 73, 73, 26, 73, 26, 74, 26, 73, 26, 73, 73, 74, 74, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 52, 0, 0, 0, 0, 52, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 0, 0, 0, 0, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 8
	mapDimensions = sf::Vector2u(13, 11);
	map = new std::vector<int>{
		1, 2, 3, 4, 11, 85, 86, 87, 11, 4, 3, 5, 6, 15, 16, 17, 142, 25, 99, 100, 101, 25, 142, 17, 19, 20, 29, 30, 80, 80, 81, 31, 31, 79, 80, 81, 31, 33, 34, 29, 148, 80, 80, 81, 31, 31, 79, 80, 80, 66, 144, 34, 43, 147, 80, 80, 95, 31, 31, 79, 80, 94, 94, 143, 48, 27, 28, 80, 81, 65, 66, 66, 80, 80, 66, 66, 13, 14, 43, 147, 80, 81, 79, 80, 80, 80, 80, 94, 94, 143, 48, 29, 148, 94, 95, 79, 80, 80, 80, 81, 31, 31, 144, 34, 29, 30, 31, 31, 79, 80, 80, 80, 81, 31, 31, 33, 34, 57, 58, 59, 146, 12, 113, 114, 115, 12, 146, 59, 61, 62, 71, 72, 73, 74, 26, 127, 128, 129, 26, 74, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 9
	mapDimensions = sf::Vector2u(19, 17);
	map = new std::vector<int>{
		1, 2, 3, 4, 4, 4, 3, 3, 11, 4, 11, 3, 3, 4, 4, 4, 3, 5, 6, 15, 16, 141, 142, 142, 142, 141, 141, 25, 142, 25, 141, 141, 142, 142, 142, 141, 19, 20, 29, 148, 80, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 80, 144, 34, 27, 28, 81, 31, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 31, 79, 13, 14, 29, 148, 81, 31, 21, 153, 153, 153, 153, 153, 153, 153, 153, 153, 23, 31, 79, 144, 34, 43, 147, 81, 31, 21, 153, 22, 22, 153, 153, 153, 22, 22, 153, 23, 31, 79, 143, 48, 43, 147, 81, 31, 21, 153, 22, 153, 23, 31, 21, 153, 22, 153, 23, 31, 79, 143, 48, 27, 28, 81, 31, 21, 153, 22, 153, 23, 31, 21, 153, 22, 153, 23, 31, 79, 144, 34, 27, 28, 81, 31, 21, 153, 22, 153, 23, 31, 21, 153, 22, 153, 23, 31, 79, 144, 34, 27, 28, 81, 31, 21, 153, 22, 153, 23, 31, 21, 153, 22, 153, 23, 31, 79, 144, 34, 88, 89, 81, 31, 21, 153, 22, 153, 23, 31, 21, 153, 22, 153, 23, 31, 79, 143, 48, 102, 103, 81, 31, 21, 153, 22, 153, 23, 31, 21, 153, 22, 153, 23, 31, 79, 143, 48, 116, 117, 81, 31, 21, 153, 22, 153, 23, 31, 21, 153, 22, 153, 23, 31, 79, 144, 34, 43, 147, 81, 31, 21, 153, 22, 22, 23, 31, 35, 36, 36, 36, 37, 31, 79, 13, 14, 27, 28, 80, 67, 21, 153, 22, 153, 23, 65, 66, 66, 66, 66, 66, 66, 80, 144, 34, 57, 58, 145, 145, 145, 145, 146, 146, 113, 114, 115, 146, 146, 145, 145, 145, 145, 61, 62, 71, 72, 73, 73, 73, 73, 74, 74, 127, 128, 129, 74, 74, 73, 73, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 10
	mapDimensions = sf::Vector2u(33, 13);
	map = new std::vector<int>{
		1, 2, 11, 11, 3, 11, 11, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 11, 3, 11, 11, 3, 5, 6, 15, 16, 25, 25, 141, 25, 25, 141, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 17, 17, 17, 17, 141, 25, 141, 25, 25, 17, 19, 20, 43, 44, 93, 80, 80, 80, 80, 95, 16, 17, 4, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 4, 17, 19, 79, 80, 80, 80, 80, 80, 47, 48, 43, 44, 31, 79, 80, 80, 81, 31, 147, 16, 142, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 142, 19, 143, 79, 80, 80, 80, 80, 80, 47, 48, 43, 147, 31, 93, 94, 94, 95, 31, 148, 148, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 144, 144, 93, 80, 80, 80, 80, 80, 143, 48, 88, 89, 31, 31, 31, 31, 31, 31, 107, 107, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 106, 106, 31, 93, 94, 94, 80, 80, 90, 91, 102, 103, 31, 31, 31, 31, 31, 31, 107, 107, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 106, 106, 31, 31, 31, 31, 93, 94, 104, 105, 116, 117, 31, 65, 66, 67, 31, 31, 107, 107, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 106, 106, 31, 65, 66, 66, 67, 31, 118, 119, 43, 147, 66, 80, 80, 80, 67, 31, 148, 148, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 144, 144, 65, 80, 80, 80, 81, 31, 143, 48, 43, 44, 80, 80, 80, 80, 80, 67, 147, 58, 146, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 60, 61, 143, 79, 80, 80, 80, 81, 31, 47, 48, 43, 44, 80, 80, 80, 80, 80, 81, 58, 59, 74, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 74, 59, 61, 79, 80, 80, 80, 81, 31, 47, 48, 57, 58, 59, 12, 12, 12, 12, 145, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60, 60, 59, 59, 59, 59, 12, 145, 12, 12, 59, 59, 61, 62, 71, 72, 73, 26, 26, 26, 26, 73, 73, 73, 73, 73, 74, 74, 74, 74, 74, 74, 74, 74, 74, 73, 73, 73, 73, 26, 73, 26, 26, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 52, 52, 52, 0, 52, 0, 52, 52, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 52, 52, 52, 0, 52, 0, 52, 52, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 38, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 38, 38, 38, 0, 38, 0, 38, 38, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 38, 38, 38, 0, 38, 0, 38, 38, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 11
	mapDimensions = sf::Vector2u(31, 40);
	map = new std::vector<int>{
		1, 2, 3, 11, 11, 11, 11, 3, 3, 4, 4, 3, 11, 3, 85, 86, 87, 3, 3, 3, 4, 4, 3, 3, 11, 11, 11, 11, 3, 5, 6, 15, 16, 141, 25, 25, 25, 25, 141, 141, 18, 18, 141, 25, 17, 99, 100, 101, 17, 141, 141, 18, 18, 141, 141, 25, 25, 25, 25, 141, 19, 20, 43, 147, 93, 94, 80, 80, 80, 81, 21, 153, 22, 23, 79, 80, 80, 80, 80, 81, 31, 21, 22, 153, 23, 79, 80, 80, 80, 80, 80, 143, 48, 43, 147, 31, 31, 93, 94, 94, 95, 21, 153, 153, 23, 93, 94, 80, 80, 80, 95, 31, 21, 153, 153, 23, 79, 80, 80, 80, 80, 80, 13, 14, 43, 147, 66, 66, 67, 31, 31, 31, 21, 153, 22, 23, 8, 9, 79, 80, 81, 7, 8, 21, 22, 153, 23, 79, 80, 80, 80, 80, 80, 13, 14, 27, 28, 80, 80, 80, 66, 66, 67, 21, 153, 153, 153, 22, 23, 79, 80, 81, 21, 22, 153, 153, 153, 23, 79, 80, 80, 80, 80, 80, 13, 14, 27, 28, 80, 80, 80, 80, 80, 81, 21, 153, 153, 153, 153, 23, 79, 80, 81, 21, 153, 153, 153, 153, 23, 93, 94, 80, 80, 80, 80, 13, 14, 27, 28, 80, 80, 80, 80, 80, 81, 35, 36, 21, 153, 22, 23, 79, 80, 81, 21, 22, 153, 23, 36, 37, 31, 65, 80, 80, 80, 80, 13, 14, 27, 28, 80, 80, 80, 80, 80, 95, 31, 31, 21, 153, 153, 23, 93, 94, 95, 21, 153, 153, 23, 65, 66, 67, 79, 80, 80, 80, 80, 13, 14, 27, 28, 80, 80, 80, 80, 81, 31, 31, 31, 21, 153, 153, 23, 31, 31, 31, 21, 153, 153, 23, 79, 80, 81, 79, 80, 80, 80, 80, 13, 14, 43, 147, 94, 94, 94, 94, 95, 31, 31, 31, 21, 153, 153, 23, 31, 31, 31, 21, 153, 153, 23, 93, 94, 95, 93, 94, 94, 94, 94, 143, 48, 43, 147, 8, 8, 9, 31, 7, 8, 8, 8, 21, 153, 22, 23, 31, 31, 31, 21, 22, 153, 23, 8, 8, 8, 9, 31, 7, 8, 8, 143, 48, 29, 30, 153, 153, 23, 31, 21, 153, 153, 153, 153, 153, 153, 23, 31, 65, 67, 21, 153, 153, 153, 153, 153, 153, 23, 31, 21, 153, 153, 33, 34, 29, 30, 153, 153, 23, 31, 21, 153, 153, 153, 153, 153, 153, 23, 31, 79, 81, 21, 153, 153, 153, 153, 153, 153, 23, 31, 21, 153, 153, 33, 34, 43, 147, 36, 36, 37, 31, 35, 36, 36, 36, 36, 36, 36, 37, 31, 79, 81, 35, 36, 36, 36, 36, 36, 36, 37, 31, 35, 36, 36, 143, 48, 43, 147, 31, 65, 66, 67, 31, 31, 31, 31, 31, 31, 65, 66, 66, 80, 81, 31, 31, 31, 31, 31, 31, 65, 66, 67, 31, 65, 66, 143, 48, 43, 147, 31, 79, 80, 80, 66, 66, 66, 66, 66, 66, 80, 80, 94, 80, 80, 66, 66, 66, 67, 31, 31, 79, 80, 80, 66, 80, 80, 13, 14, 43, 147, 31, 79, 80, 80, 80, 80, 80, 80, 80, 80, 80, 81, 31, 79, 80, 80, 80, 80, 80, 66, 66, 80, 80, 80, 80, 80, 80, 13, 14, 43, 147, 31, 93, 94, 94, 94, 94, 94, 94, 80, 80, 80, 81, 31, 79, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 143, 48, 57, 58, 145, 151, 16, 141, 141, 141, 141, 141, 11, 92, 92, 92, 11, 141, 11, 92, 92, 92, 11, 141, 141, 141, 141, 141, 19, 152, 145, 61, 62, 71, 72, 138, 139, 44, 16, 18, 4, 4, 18, 25, 92, 92, 92, 25, 4, 25, 92, 92, 92, 25, 18, 4, 4, 18, 19, 47, 140, 112, 75, 76, 0, 0, 29, 30, 29, 30, 1, 18, 18, 4, 11, 92, 92, 92, 11, 4, 11, 92, 92, 92, 11, 4, 18, 18, 6, 33, 34, 33, 34, 0, 0, 0, 0, 29, 30, 29, 30, 15, 16, 141, 142, 25, 92, 92, 92, 25, 18, 25, 92, 92, 92, 25, 142, 141, 19, 20, 33, 33, 34, 34, 0, 0, 0, 0, 29, 29, 30, 30, 43, 147, 31, 65, 67, 31, 31, 21, 22, 22, 22, 23, 79, 80, 80, 80, 81, 143, 48, 33, 34, 33, 34, 0, 0, 0, 0, 29, 30, 29, 30, 88, 89, 31, 79, 81, 31, 31, 21, 153, 22, 153, 23, 79, 80, 80, 80, 95, 90, 91, 33, 33, 34, 34, 0, 0, 0, 0, 29, 29, 30, 30, 102, 103, 31, 79, 80, 67, 31, 21, 153, 22, 153, 23, 93, 94, 94, 95, 31, 104, 105, 33, 34, 33, 34, 0, 0, 0, 0, 29, 30, 29, 30, 116, 117, 31, 79, 80, 80, 67, 21, 153, 153, 153, 23, 31, 65, 67, 31, 31, 118, 119, 33, 33, 34, 34, 0, 0, 0, 0, 29, 29, 30, 30, 43, 147, 31, 79, 80, 80, 80, 67, 36, 36, 36, 37, 31, 93, 80, 67, 31, 143, 48, 33, 34, 33, 34, 0, 0, 0, 0, 29, 30, 29, 30, 43, 147, 31, 93, 94, 94, 94, 95, 31, 31, 31, 31, 31, 31, 93, 95, 31, 143, 48, 33, 33, 34, 34, 0, 0, 0, 0, 29, 29, 30, 30, 43, 147, 8, 8, 8, 8, 8, 9, 65, 67, 31, 7, 8, 8, 8, 8, 8, 143, 48, 33, 34, 33, 34, 0, 0, 0, 0, 29, 29, 30, 30, 43, 44, 153, 153, 153, 153, 22, 23, 79, 81, 31, 21, 22, 153, 153, 153, 153, 47, 48, 33, 33, 34, 34, 0, 0, 0, 0, 29, 30, 29, 30, 43, 44, 22, 22, 22, 22, 153, 23, 79, 81, 31, 21, 153, 22, 22, 22, 22, 47, 48, 33, 34, 33, 34, 0, 0, 0, 0, 29, 29, 30, 30, 43, 44, 22, 22, 22, 22, 153, 23, 79, 81, 31, 21, 153, 22, 22, 22, 22, 47, 48, 33, 33, 34, 34, 0, 0, 0, 0, 29, 30, 29, 30, 43, 44, 22, 22, 22, 22, 153, 23, 79, 80, 67, 21, 153, 22, 22, 22, 22, 47, 48, 33, 34, 33, 34, 0, 0, 0, 0, 29, 29, 30, 30, 57, 58, 59, 59, 59, 59, 59, 145, 113, 114, 115, 145, 59, 59, 59, 59, 59, 61, 62, 33, 33, 34, 34, 0, 0, 0, 0, 29, 30, 29, 30, 71, 72, 73, 73, 73, 73, 73, 73, 127, 128, 129, 73, 73, 73, 73, 73, 73, 75, 76, 33, 34, 33, 34, 0, 0, 0, 0, 29, 30, 57, 71, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 76, 62, 33, 34, 0, 0, 0, 0, 29, 30, 71, 72, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 75, 76, 33, 34, 0, 0, 0, 0, 57, 71, 60, 60, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 74, 60, 60, 60, 76, 62, 0, 0, 0, 0, 71, 72, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 75, 76, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 52, 52, 0, 52, 0, 0, 52, 0, 0, 52, 0, 52, 52, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 38, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 38, 38, 0, 38, 39, 39, 38, 39, 39, 38, 0, 38, 38, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 12
	mapDimensions = sf::Vector2u(30, 28);
	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 4, 4, 85, 86, 87, 4, 4, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 18, 18, 18, 99, 100, 101, 18, 18, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 22, 22, 153, 23, 31, 21, 153, 22, 22, 33, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 22, 22, 153, 23, 31, 21, 153, 22, 22, 33, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 153, 22, 153, 23, 31, 21, 153, 22, 153, 33, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 36, 36, 36, 37, 31, 35, 36, 36, 36, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 31, 31, 31, 31, 31, 31, 31, 31, 31, 13, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 31, 31, 31, 31, 31, 31, 31, 31, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 44, 16, 141, 92, 92, 92, 92, 92, 141, 19, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 27, 28, 80, 80, 80, 80, 80, 80, 80, 13, 14, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 44, 28, 80, 80, 80, 80, 80, 80, 80, 13, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 27, 28, 80, 80, 80, 94, 94, 80, 80, 13, 14, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 44, 28, 80, 80, 81, 31, 31, 79, 80, 13, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 27, 28, 80, 80, 81, 31, 31, 79, 80, 13, 14, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 44, 28, 80, 80, 81, 31, 31, 79, 80, 13, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 27, 28, 94, 80, 80, 66, 66, 80, 80, 13, 14, 48, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 3, 11, 3, 3, 3, 122, 123, 147, 31, 79, 80, 80, 80, 80, 80, 143, 120, 121, 3, 3, 3, 4, 4, 3, 5, 6, 15, 16, 17, 17, 17, 25, 17, 17, 11, 136, 137, 147, 31, 79, 80, 80, 80, 80, 80, 143, 134, 135, 4, 17, 141, 142, 142, 141, 19, 20, 43, 44, 1, 2, 11, 11, 11, 3, 25, 25, 141, 150, 66, 80, 80, 80, 80, 80, 80, 149, 141, 141, 142, 19, 31, 31, 31, 31, 143, 48, 43, 44, 15, 16, 25, 25, 25, 19, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 81, 31, 31, 31, 143, 31, 31, 31, 31, 143, 48, 43, 44, 88, 89, 80, 80, 80, 106, 80, 80, 80, 80, 94, 94, 80, 80, 80, 80, 80, 80, 67, 31, 31, 106, 31, 31, 31, 31, 90, 91, 43, 44, 102, 103, 80, 80, 80, 106, 80, 80, 80, 81, 31, 31, 79, 80, 80, 80, 80, 80, 81, 31, 31, 106, 31, 31, 31, 31, 104, 105, 43, 44, 116, 117, 80, 80, 80, 106, 80, 80, 80, 81, 31, 31, 79, 80, 80, 80, 80, 80, 81, 31, 31, 106, 31, 31, 31, 31, 118, 119, 43, 44, 29, 148, 80, 80, 80, 106, 80, 80, 80, 80, 66, 66, 80, 80, 94, 94, 80, 80, 81, 31, 31, 106, 31, 31, 31, 31, 143, 48, 43, 44, 57, 58, 12, 12, 12, 61, 80, 80, 80, 80, 80, 80, 80, 81, 31, 31, 79, 80, 81, 31, 31, 143, 31, 31, 31, 31, 143, 48, 43, 44, 71, 72, 26, 26, 26, 145, 145, 145, 145, 145, 146, 145, 146, 145, 146, 145, 146, 145, 145, 145, 145, 61, 31, 31, 31, 31, 143, 48, 57, 58, 59, 59, 59, 12, 59, 59, 59, 60, 59, 60, 74, 60, 74, 60, 74, 60, 74, 60, 59, 60, 59, 59, 145, 146, 146, 145, 61, 62, 71, 72, 73, 73, 73, 26, 73, 73, 73, 74, 73, 74, 73, 74, 73, 74, 73, 74, 73, 74, 73, 74, 73, 73, 73, 74, 74, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 39, 39, 39, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 13
	mapDimensions = sf::Vector2u(31, 21);
	map = new std::vector<int>{
	1, 2, 3, 3, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 3, 3, 3, 122, 123, 63, 63, 120, 121, 3, 11, 3, 3, 3, 5, 6, 15, 16, 17, 17, 17, 18, 17, 18, 17, 18, 17, 18, 17, 18, 17, 17, 17, 17, 136, 137, 63, 63, 134, 17, 17, 25, 17, 17, 17, 19, 20, 43, 44, 1, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 11, 3, 3, 3, 3, 11, 11, 11, 11, 5, 6, 47, 48, 43, 44, 15, 16, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 141, 141, 141, 25, 141, 141, 141, 141, 25, 25, 25, 25, 19, 20, 47, 48, 43, 44, 29, 30, 153, 22, 153, 22, 22, 153, 22, 22, 153, 22, 153, 23, 31, 31, 93, 80, 80, 80, 80, 80, 80, 80, 80, 13, 14, 47, 48, 29, 30, 29, 30, 153, 22, 153, 36, 36, 36, 36, 36, 153, 22, 153, 23, 31, 31, 31, 93, 94, 80, 80, 80, 80, 80, 80, 13, 14, 33, 34, 43, 44, 29, 30, 153, 22, 153, 23, 65, 66, 67, 21, 153, 22, 153, 23, 65, 66, 66, 66, 67, 93, 94, 94, 80, 80, 80, 13, 14, 33, 34, 29, 30, 29, 30, 153, 22, 153, 23, 79, 80, 81, 21, 153, 22, 153, 23, 79, 80, 94, 80, 81, 31, 31, 31, 79, 80, 80, 13, 14, 33, 34, 43, 44, 29, 30, 153, 22, 153, 23, 93, 94, 95, 21, 153, 22, 153, 23, 79, 80, 66, 80, 81, 31, 31, 31, 93, 94, 94, 13, 14, 47, 48, 29, 30, 29, 30, 153, 153, 153, 8, 9, 31, 7, 8, 153, 153, 153, 23, 93, 94, 94, 94, 95, 31, 31, 31, 31, 31, 31, 143, 48, 47, 48, 122, 44, 29, 148, 36, 36, 36, 36, 37, 31, 35, 36, 36, 36, 36, 37, 31, 65, 66, 66, 66, 66, 67, 31, 31, 31, 31, 143, 48, 47, 48, 17, 137, 29, 148, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 79, 80, 80, 80, 80, 81, 31, 31, 31, 31, 143, 48, 47, 48, 49, 49, 43, 28, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 79, 80, 80, 80, 80, 81, 31, 65, 66, 66, 143, 48, 47, 48, 49, 49, 43, 28, 66, 66, 67, 31, 31, 31, 31, 31, 31, 31, 31, 31, 65, 80, 80, 80, 80, 80, 81, 31, 79, 80, 80, 106, 106, 90, 91, 124, 125, 27, 28, 80, 80, 80, 66, 66, 66, 66, 67, 65, 66, 67, 31, 79, 80, 94, 95, 93, 80, 81, 65, 80, 94, 94, 106, 106, 104, 105, 138, 44, 27, 28, 80, 80, 80, 80, 80, 80, 80, 81, 79, 80, 80, 66, 80, 95, 31, 31, 31, 79, 81, 79, 81, 31, 31, 106, 106, 118, 119, 43, 44, 27, 28, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 81, 31, 31, 31, 31, 79, 81, 79, 81, 31, 31, 143, 48, 47, 48, 43, 44, 57, 58, 145, 12, 145, 12, 145, 12, 12, 145, 12, 145, 12, 145, 12, 12, 145, 12, 145, 12, 145, 145, 12, 12, 145, 61, 62, 47, 48, 43, 44, 71, 72, 73, 26, 73, 26, 60, 26, 26, 60, 26, 73, 26, 60, 26, 26, 60, 26, 73, 26, 73, 73, 26, 26, 73, 75, 76, 47, 48, 57, 58, 59, 59, 59, 59, 59, 59, 74, 59, 59, 74, 59, 59, 59, 74, 59, 59, 74, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 61, 62, 71, 72, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 75, 76
	};
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 38, 40, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 63, 63, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 69, 0, 0, 0, 0, 0, 0, 0, 0, 39, 39, 39, 39, 39, 39, 38, 39, 39, 38, 39, 39, 38, 39, 39, 38, 39, 39, 38, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 14
	mapDimensions = sf::Vector2u(22, 19);
	map = new std::vector<int>{
		1, 2, 3, 11, 3, 11, 3, 11, 3, 11, 3, 11, 3, 3, 3, 3, 11, 11, 3, 3, 5, 6, 15, 16, 141, 25, 141, 25, 141, 25, 141, 25, 141, 25, 141, 141, 141, 141, 25, 25, 141, 141, 19, 20, 43, 147, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 81, 31, 31, 79, 80, 80, 81, 31, 143, 48, 88, 89, 80, 80, 95, 93, 80, 80, 80, 80, 80, 80, 95, 31, 31, 79, 80, 80, 81, 31, 90, 91, 102, 103, 80, 95, 31, 31, 93, 80, 80, 80, 80, 81, 31, 65, 66, 80, 80, 80, 95, 31, 104, 105, 116, 117, 95, 31, 31, 31, 31, 93, 94, 94, 94, 95, 31, 93, 94, 94, 94, 95, 31, 31, 118, 119, 43, 147, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 144, 34, 43, 147, 153, 22, 153, 153, 22, 153, 22, 153, 22, 153, 22, 153, 22, 153, 22, 153, 22, 153, 143, 48, 29, 148, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 144, 34, 29, 148, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 48, 29, 148, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 144, 34, 43, 147, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 48, 43, 147, 36, 36, 36, 36, 36, 36, 36, 36, 36, 22, 22, 22, 22, 22, 22, 22, 22, 22, 144, 34, 43, 147, 31, 31, 31, 31, 65, 66, 67, 31, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 48, 43, 147, 31, 31, 65, 66, 80, 80, 81, 31, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 144, 34, 43, 147, 31, 65, 80, 80, 80, 80, 81, 31, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 48, 43, 147, 65, 80, 80, 80, 80, 80, 81, 31, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 144, 34, 57, 58, 145, 12, 145, 12, 145, 12, 145, 145, 145, 146, 146, 146, 145, 145, 145, 145, 146, 146, 61, 62, 71, 72, 73, 26, 73, 26, 73, 26, 73, 73, 73, 74, 74, 74, 138, 63, 63, 126, 74, 74, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 38, 0, 38, 0, 38, 0, 38, 0, 38, 0, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 15
	mapDimensions = sf::Vector2u(20, 9);
	map = new std::vector<int>{
		1, 2, 3, 4, 3, 4, 3, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 6, 15, 16, 17, 142, 17, 142, 17, 142, 17, 141, 17, 11, 17, 11, 17, 11, 17, 17, 19, 20, 43, 44, 31, 31, 31, 31, 31, 31, 31, 31, 16, 25, 17, 25, 17, 25, 17, 19, 47, 48, 88, 89, 31, 65, 66, 66, 66, 67, 31, 31, 107, 80, 80, 80, 80, 80, 80, 90, 91, 48, 102, 103, 31, 79, 80, 80, 80, 80, 67, 31, 107, 80, 80, 80, 80, 80, 80, 104, 105, 48, 116, 117, 31, 79, 80, 80, 80, 80, 95, 31, 107, 80, 80, 80, 80, 80, 80, 118, 119, 48, 43, 44, 65, 80, 80, 80, 80, 81, 31, 31, 58, 12, 145, 12, 145, 12, 145, 61, 47, 48, 57, 58, 59, 146, 59, 146, 59, 146, 59, 145, 59, 26, 59, 26, 59, 26, 59, 59, 61, 62, 71, 72, 73, 74, 73, 74, 73, 74, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 52, 0, 52, 0, 0, 0, 52, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 38, 0, 38, 39, 38, 0, 0, 0, 38, 0, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 38, 0, 38, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 16
	mapDimensions = sf::Vector2u(36, 24);
	map = new std::vector<int>{
		1, 2, 3, 3, 4, 4, 4, 4, 4, 3, 3, 3, 4, 4, 3, 4, 4, 3, 4, 4, 3, 3, 4, 4, 3, 3, 3, 4, 4, 4, 4, 3, 3, 3, 5, 6, 15, 16, 17, 17, 142, 142, 142, 142, 142, 17, 17, 17, 18, 18, 17, 18, 18, 17, 18, 18, 17, 17, 142, 142, 17, 17, 17, 142, 142, 142, 142, 17, 17, 17, 19, 20, 43, 44, 1, 2, 4, 4, 4, 4, 4, 3, 3, 3, 3, 4, 3, 4, 4, 3, 4, 3, 3, 3, 11, 11, 3, 3, 3, 11, 11, 11, 11, 3, 5, 6, 47, 48, 43, 44, 15, 16, 142, 142, 142, 142, 142, 141, 141, 141, 141, 142, 141, 142, 142, 141, 142, 141, 141, 17, 25, 25, 17, 141, 17, 25, 25, 25, 25, 17, 19, 20, 47, 48, 43, 44, 43, 147, 80, 80, 80, 80, 80, 81, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 79, 80, 80, 81, 31, 79, 80, 80, 80, 80, 81, 144, 34, 47, 48, 29, 30, 29, 148, 80, 80, 80, 80, 80, 81, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 79, 80, 80, 81, 31, 93, 80, 80, 80, 80, 81, 144, 34, 33, 34, 43, 44, 29, 148, 80, 80, 80, 80, 80, 81, 31, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 79, 80, 80, 80, 66, 66, 80, 80, 80, 80, 81, 144, 34, 47, 48, 29, 30, 43, 147, 93, 80, 80, 80, 80, 81, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 93, 80, 80, 80, 80, 80, 94, 94, 80, 80, 81, 90, 91, 47, 48, 29, 30, 29, 148, 31, 93, 80, 80, 80, 81, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 31, 93, 80, 80, 80, 81, 31, 31, 79, 80, 81, 104, 105, 33, 34, 29, 30, 43, 147, 31, 31, 79, 80, 80, 81, 31, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 31, 31, 93, 80, 80, 81, 31, 31, 79, 80, 95, 118, 119, 47, 48, 29, 30, 29, 148, 31, 31, 79, 80, 80, 81, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 31, 31, 31, 93, 80, 80, 66, 66, 80, 95, 31, 144, 34, 47, 48, 43, 44, 43, 147, 31, 31, 93, 94, 94, 95, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 65, 66, 66, 67, 93, 94, 94, 94, 95, 31, 65, 144, 34, 33, 34, 43, 44, 43, 147, 67, 31, 31, 31, 31, 31, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 79, 80, 80, 80, 67, 31, 31, 31, 31, 65, 80, 144, 34, 47, 48, 29, 30, 27, 28, 80, 67, 31, 31, 31, 31, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 23, 79, 80, 80, 80, 81, 31, 31, 31, 31, 79, 80, 143, 48, 47, 48, 43, 29, 30, 28, 80, 80, 67, 31, 31, 31, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 152, 145, 145, 145, 146, 60, 113, 114, 115, 60, 146, 145, 61, 62, 47, 48, 29, 30, 27, 28, 80, 80, 80, 66, 67, 31, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 143, 112, 73, 73, 74, 74, 127, 128, 129, 74, 74, 73, 75, 76, 47, 48, 43, 29, 30, 28, 80, 80, 80, 80, 81, 31, 31, 21, 153, 22, 22, 22, 22, 22, 22, 153, 143, 48, 126, 98, 59, 60, 59, 60, 59, 60, 59, 59, 59, 59, 61, 62, 29, 30, 27, 28, 80, 80, 80, 80, 80, 66, 67, 21, 153, 22, 22, 22, 22, 22, 22, 153, 143, 48, 140, 112, 73, 74, 73, 74, 73, 74, 73, 73, 73, 73, 75, 76, 43, 29, 30, 28, 80, 80, 80, 80, 80, 80, 81, 21, 153, 22, 22, 22, 22, 22, 22, 153, 143, 48, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 30, 27, 28, 80, 80, 80, 80, 80, 80, 81, 21, 153, 22, 22, 22, 22, 22, 22, 153, 143, 48, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 44, 57, 58, 12, 145, 12, 59, 12, 145, 145, 145, 146, 146, 145, 59, 59, 145, 146, 146, 61, 62, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 44, 71, 72, 26, 73, 26, 73, 26, 73, 73, 73, 74, 74, 73, 73, 73, 73, 74, 74, 75, 76, 47, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 57, 58, 59, 59, 59, 60, 60, 60, 59, 59, 59, 59, 59, 124, 125, 63, 63, 126, 98, 59, 59, 59, 61, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 72, 73, 73, 73, 74, 74, 74, 73, 73, 73, 73, 73, 138, 139, 63, 63, 140, 112, 73, 73, 73, 75, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 38, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 17
	mapDimensions = sf::Vector2u(13, 20);
	map = new std::vector<int>{
		1, 2, 3, 3, 4, 85, 86, 87, 4, 3, 3, 5, 6, 15, 16, 17, 17, 18, 99, 100, 101, 18, 17, 17, 19, 20, 43, 44, 16, 141, 141, 92, 92, 92, 141, 141, 19, 47, 48, 43, 44, 147, 80, 80, 80, 80, 80, 80, 80, 143, 47, 48, 43, 27, 28, 80, 80, 94, 94, 94, 80, 80, 13, 14, 48, 43, 27, 28, 80, 81, 7, 8, 9, 79, 80, 13, 14, 48, 43, 44, 147, 80, 81, 21, 22, 23, 79, 80, 143, 47, 48, 43, 27, 28, 80, 81, 35, 36, 37, 79, 80, 13, 14, 48, 43, 27, 28, 80, 80, 66, 66, 66, 80, 80, 13, 14, 48, 43, 44, 147, 80, 80, 80, 80, 80, 80, 80, 143, 47, 48, 43, 44, 58, 108, 108, 145, 145, 145, 108, 108, 61, 47, 48, 43, 147, 66, 67, 31, 7, 8, 9, 31, 31, 31, 143, 48, 27, 28, 80, 80, 67, 21, 22, 23, 65, 66, 66, 13, 14, 27, 28, 80, 80, 81, 35, 36, 37, 79, 80, 80, 13, 14, 43, 147, 80, 80, 95, 31, 31, 31, 79, 80, 80, 143, 48, 43, 147, 94, 95, 65, 66, 66, 66, 80, 80, 80, 143, 48, 43, 147, 31, 31, 79, 80, 80, 80, 80, 80, 80, 143, 48, 43, 147, 31, 31, 79, 80, 80, 80, 80, 80, 80, 143, 48, 57, 58, 145, 145, 60, 113, 114, 115, 60, 145, 145, 61, 62, 71, 72, 73, 73, 74, 127, 128, 129, 74, 73, 73, 75, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 39, 38, 0, 38, 39, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 38, 0, 38, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 18
	mapDimensions = sf::Vector2u(28, 32);
	map = new std::vector<int>{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 11, 11, 11, 85, 86, 87, 4, 4, 4, 3, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 16, 141, 25, 25, 25, 99, 100, 101, 142, 142, 142, 141, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 80, 80, 80, 80, 80, 80, 81, 31, 31, 31, 31, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 80, 80, 94, 94, 94, 94, 95, 31, 31, 31, 31, 144, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 80, 81, 7, 8, 8, 8, 8, 8, 9, 31, 31, 144, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 80, 81, 21, 153, 153, 153, 153, 153, 23, 31, 65, 144, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 94, 95, 21, 22, 22, 22, 22, 22, 23, 65, 80, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 31, 21, 153, 22, 22, 22, 153, 23, 79, 80, 13, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 31, 31, 21, 153, 22, 22, 22, 153, 23, 79, 80, 13, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 147, 67, 31, 21, 153, 22, 22, 22, 153, 23, 93, 80, 143, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 80, 67, 21, 153, 22, 22, 22, 153, 23, 31, 93, 144, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 80, 81, 21, 22, 153, 22, 153, 22, 23, 31, 31, 144, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 28, 80, 81, 35, 36, 153, 22, 153, 36, 37, 31, 31, 144, 34, 1, 2, 3, 122, 63, 63, 121, 3, 3, 3, 3, 3, 3, 122, 147, 94, 95, 31, 21, 153, 22, 153, 23, 31, 31, 31, 143, 48, 15, 16, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 150, 8, 8, 8, 8, 153, 22, 153, 8, 8, 8, 8, 143, 48, 43, 147, 80, 80, 80, 80, 80, 67, 21, 153, 22, 22, 22, 22, 22, 153, 153, 153, 153, 153, 22, 153, 153, 153, 153, 153, 143, 48, 43, 147, 94, 94, 94, 94, 80, 81, 21, 153, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 48, 43, 147, 31, 31, 31, 31, 79, 81, 21, 153, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 121, 43, 147, 31, 31, 31, 31, 79, 81, 21, 153, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 49, 29, 148, 67, 31, 31, 31, 79, 81, 21, 153, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 49, 27, 28, 80, 66, 66, 66, 80, 81, 21, 153, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 126, 29, 148, 80, 80, 80, 80, 80, 81, 21, 153, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 143, 48, 29, 148, 93, 80, 80, 80, 80, 95, 21, 153, 22, 22, 22, 22, 153, 153, 153, 153, 22, 153, 153, 153, 22, 153, 153, 153, 143, 48, 29, 148, 31, 93, 94, 94, 95, 31, 21, 153, 22, 22, 22, 22, 153, 23, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 143, 48, 29, 148, 31, 31, 31, 31, 31, 31, 21, 153, 22, 22, 22, 22, 153, 23, 31, 31, 31, 65, 66, 66, 67, 31, 31, 31, 144, 34, 29, 148, 31, 31, 65, 66, 66, 67, 21, 22, 22, 22, 22, 22, 22, 23, 31, 31, 65, 80, 80, 80, 80, 66, 66, 66, 144, 34, 43, 147, 31, 65, 80, 80, 80, 81, 21, 153, 22, 22, 22, 22, 153, 23, 31, 65, 80, 80, 80, 80, 80, 80, 80, 80, 90, 91, 43, 147, 65, 80, 80, 80, 80, 81, 21, 153, 22, 22, 22, 22, 153, 23, 31, 79, 80, 80, 80, 80, 94, 94, 80, 80, 104, 105, 43, 147, 79, 80, 80, 80, 80, 81, 21, 22, 22, 22, 22, 22, 22, 23, 31, 79, 80, 80, 80, 95, 31, 31, 79, 80, 118, 119, 43, 147, 79, 80, 80, 80, 80, 81, 21, 153, 22, 22, 22, 22, 153, 23, 31, 79, 80, 80, 81, 31, 31, 31, 79, 80, 144, 34, 57, 58, 145, 12, 12, 12, 12, 145, 145, 145, 12, 12, 12, 12, 145, 145, 145, 145, 12, 12, 145, 146, 146, 146, 145, 145, 61, 62, 71, 72, 73, 26, 26, 26, 26, 73, 73, 73, 26, 26, 26, 26, 73, 73, 73, 73, 26, 26, 73, 74, 74, 74, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 63, 63, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 63, 63, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 63, 63, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 19
	mapDimensions = sf::Vector2u(35, 10);
	map = new std::vector<int>{
		1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 6, 15, 16, 141, 17, 141, 17, 141, 17, 141, 17, 141, 17, 141, 17, 141, 17, 11, 11, 11, 11, 11, 11, 11, 11, 11, 3, 3, 3, 3, 3, 3, 3, 5, 6, 20, 43, 147, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 16, 25, 25, 25, 25, 25, 25, 25, 25, 25, 17, 17, 141, 141, 17, 17, 141, 19, 20, 48, 88, 89, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 107, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 90, 91, 48, 102, 103, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 107, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 104, 105, 48, 116, 117, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 107, 31, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 118, 119, 48, 43, 147, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 107, 31, 21, 22, 153, 153, 22, 22, 153, 153, 22, 22, 153, 153, 22, 22, 153, 13, 14, 48, 43, 147, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 58, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 61, 62, 48, 57, 58, 145, 59, 145, 59, 145, 59, 145, 59, 145, 59, 145, 59, 145, 59, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 75, 76, 62, 71, 72, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 0, 0, 0, 52, 0, 0, 52, 52, 0, 0, 52, 52, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 52, 0, 0, 52, 52, 0, 0, 52, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 38, 0, 38, 39, 38, 0, 38, 39, 38, 0, 0, 0, 0, 38, 39, 39, 38, 38, 39, 39, 38, 38, 39, 39, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 38, 38, 39, 39, 38, 38, 39, 39, 38, 38, 0, 0, 0, 0, 0, 0, 0, 38, 39, 38, 0, 38, 39, 38, 0, 38, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 20
	mapDimensions = sf::Vector2u(13, 12);
	map = new std::vector<int>{
		1, 2, 3, 11, 11, 85, 86, 87, 11, 11, 3, 5, 6, 15, 16, 17, 25, 25, 99, 100, 101, 25, 25, 17, 19, 20, 27, 28, 80, 80, 80, 80, 80, 80, 80, 80, 80, 13, 14, 43, 147, 80, 80, 80, 80, 80, 80, 80, 94, 94, 143, 48, 27, 28, 80, 80, 94, 94, 94, 94, 95, 31, 31, 13, 14, 29, 148, 80, 81, 31, 31, 31, 31, 31, 31, 31, 144, 34, 29, 30, 80, 81, 31, 31, 31, 31, 31, 31, 31, 33, 34, 27, 28, 80, 80, 66, 66, 67, 31, 31, 31, 31, 13, 14, 43, 147, 80, 80, 80, 80, 80, 66, 66, 66, 66, 143, 48, 27, 28, 80, 80, 80, 80, 80, 80, 80, 80, 80, 13, 14, 57, 58, 59, 12, 12, 113, 114, 115, 12, 12, 59, 61, 62, 71, 72, 73, 26, 26, 127, 128, 129, 26, 26, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 52, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 38, 38, 38, 39, 39, 38, 0, 0, 0, 0, 38, 39, 39, 38, 38, 38, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	//Room 21
	mapDimensions = sf::Vector2u(27, 17);
	map = new std::vector<int>{
		1, 2, 3, 3, 3, 4, 4, 4, 3, 3, 11, 11, 85, 86, 87, 11, 11, 3, 3, 4, 4, 4, 3, 3, 3, 5, 6, 15, 16, 17, 17, 141, 142, 142, 142, 141, 141, 25, 25, 99, 100, 101, 25, 25, 17, 141, 142, 142, 142, 141, 17, 17, 19, 20, 43, 44, 79, 80, 80, 81, 31, 31, 31, 31, 79, 81, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 79, 80, 80, 47, 48, 29, 30, 79, 80, 80, 81, 31, 31, 31, 31, 79, 81, 31, 31, 31, 31, 31, 31, 65, 66, 66, 67, 79, 80, 80, 33, 34, 29, 148, 93, 80, 80, 80, 66, 67, 31, 31, 79, 95, 31, 31, 31, 31, 31, 65, 80, 80, 80, 81, 93, 94, 94, 144, 34, 43, 147, 31, 93, 94, 94, 80, 80, 66, 66, 81, 31, 31, 31, 31, 31, 65, 80, 80, 80, 80, 81, 31, 31, 31, 143, 48, 43, 147, 31, 31, 31, 31, 93, 94, 94, 94, 95, 31, 31, 31, 31, 65, 80, 80, 80, 80, 80, 81, 31, 31, 31, 143, 48, 43, 44, 16, 141, 92, 92, 92, 141, 11, 11, 17, 11, 11, 11, 11, 11, 17, 11, 11, 141, 92, 92, 92, 141, 19, 47, 48, 43, 44, 44, 16, 92, 92, 92, 141, 25, 25, 17, 25, 25, 25, 25, 25, 17, 25, 25, 141, 92, 92, 92, 19, 47, 47, 48, 43, 44, 44, 147, 31, 31, 31, 79, 80, 80, 81, 31, 31, 31, 31, 31, 79, 80, 80, 81, 31, 31, 31, 143, 47, 47, 48, 29, 30, 29, 147, 31, 31, 31, 79, 80, 80, 81, 31, 31, 31, 31, 31, 79, 80, 80, 81, 31, 31, 31, 144, 34, 33, 34, 29, 30, 29, 147, 31, 31, 31, 93, 80, 80, 80, 66, 66, 66, 66, 66, 80, 80, 80, 95, 31, 31, 31, 144, 34, 33, 34, 43, 44, 43, 147, 31, 31, 31, 31, 93, 80, 80, 80, 80, 80, 80, 80, 80, 80, 95, 31, 31, 31, 31, 47, 48, 47, 48, 43, 44, 57, 58, 59, 12, 145, 146, 146, 145, 59, 146, 113, 114, 115, 146, 59, 145, 146, 146, 145, 12, 59, 61, 62, 47, 48, 43, 44, 71, 72, 73, 26, 73, 74, 74, 73, 73, 74, 127, 128, 129, 74, 73, 73, 74, 74, 73, 26, 73, 75, 76, 47, 48, 57, 58, 59, 59, 59, 60, 59, 60, 60, 59, 59, 60, 60, 60, 60, 60, 59, 59, 60, 60, 59, 60, 59, 59, 59, 61, 62, 71, 72, 73, 73, 73, 74, 73, 74, 74, 73, 73, 74, 74, 74, 74, 74, 73, 73, 74, 74, 73, 74, 73, 73, 73, 75, 76 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L1, &tileSet);

	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 52, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L2, &tileSet);
	
	map = new std::vector<int>{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 38, 0, 0, 38, 39, 39, 38, 39, 39, 38, 0, 0, 38, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 38, 0, 0, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 38, 39, 39, 39, 39, 39, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	setRoom(map, mapDimensions, &roomsMaps_F1_L3, &tileSet);

	for (int i = 0; i < roomsMaps_F1_L1.size(); i++) {     //iterate through all the room layers stored in container for Layer 1
		std::vector<RoomMap> d;                            //create a dummy vector
		d.push_back(roomsMaps_F1_L1[i]);                   //push the current room's layer 1 into the dummy
		rooms[0].push_back(d);                             //push th edummy into the proper location in 3D vector
	}
	for (int i = 0; i < roomsMaps_F1_L2.size(); i++) {     //iterate through all rooms' layer 2 container...
		rooms[0][i].push_back(roomsMaps_F1_L2[i]);         //...and push current room's layer 2 and... 
		rooms[0][i].push_back(roomsMaps_F1_L3[i]);         //... and layer 3 into the orect place in the 3D vector
	}
}

void RoomLayouts::setRoom(std::vector<int>* map, sf::Vector2u mapDimension, std::vector<RoomMap>* storage, TileSet* tileSet)
{
	//initialises the roomMap object using the parameters
	RoomMap tileMap;
	tileMap.loadTexture(tileSet->getTexture());
	tileMap.setTileSet(tileSet->getTileSet());
	tileMap.setTileMap(*map, mapDimension);
	storage->push_back(tileMap);                   //pushes the created room in to the container
	delete map;                                    //deletes the map vector to be redefined

}

void RoomLayouts::setDoors(int floorIndex)
{
	switch (floorIndex) {
	case 0:
		//generates the doors in all the rooms on a given floor 
		generateDoors(0);

		//locks the doors that shold be locked
		doorManager.getDoorsInRoom(0,4)[0]->lock(); //example: lock door 0 on floor 0, room 4
		doorManager.getDoorsInRoom(0,20)[0]->lock();
		doorManager.getDoorsInRoom(0,11)[0]->lock();
		doorManager.getDoorsInRoom(0,21)[0]->lock();
		//determines the special doors
		doorManager.getDoorsInRoom(0, 1)[1]->makeDoorType(DoorType::BACK_TO_MENU); //example: set the door 1 on floor 0, room 1 to return user to the main menu
		doorManager.getDoorsInRoom(0, 21)[0]->makeDoorType(DoorType::END);
		//links the doors between rooms
		doorManager.linkDoors(0, 1, 0, 0, 2, 1);   //example: link door 0 on floor 0,room 1 with door 1 on floor 0, room 2 and vice versa
		doorManager.linkDoors(0, 2, 0, 0, 3, 2);
		doorManager.linkDoors(0, 3, 0, 0, 19, 1);
		doorManager.linkDoors(0, 3, 1, 0, 4, 1);
		doorManager.linkDoors(0, 4, 0, 0, 5, 2);
		doorManager.linkDoors(0, 5, 0, 0, 8, 1);
		doorManager.linkDoors(0, 5, 1, 0, 6, 1);
		doorManager.linkDoors(0, 6, 0, 0, 7, 0);
		doorManager.linkDoors(0, 7, 1, 0, 12, 2);
		doorManager.linkDoors(0, 8, 0, 0, 9, 1);
		doorManager.linkDoors(0, 9, 0, 0, 10, 1);
		doorManager.linkDoors(0, 10, 0, 0, 11, 2);
		doorManager.linkDoors(0, 11, 0, 0, 20, 1);
		doorManager.linkDoors(0, 11, 1, 0, 14, 1);
		doorManager.linkDoors(0, 11, 3, 0, 12, 0);
		doorManager.linkDoors(0, 12, 1, 0, 13, 0);
		doorManager.linkDoors(0, 14, 0, 0, 15, 1);
		doorManager.linkDoors(0, 15, 0, 0, 16, 0);
		doorManager.linkDoors(0, 16, 1, 0, 17, 0);
		doorManager.linkDoors(0, 17, 1, 0, 18, 0);
		doorManager.linkDoors(0, 18, 1, 0, 19, 0);
		doorManager.linkDoors(0, 20, 0, 0, 21, 1);
		break;
	default:
		break;
	}
}

void RoomLayouts::addDoor(int roomIndex,int floorIndex, int tileIndex , DoorSide side, bool isLocked)
{
	std::vector<GameObject>* currentRoom;                               //holder for the current room to reduce access time
	GameObject tile;                                                    //holder for the cuurrent tile to reduce access time

	currentRoom = rooms[floorIndex][roomIndex-1][0].getLevel();         //get the map of layer 0 of the desired room
	tile = (*currentRoom)[tileIndex];                                   //get a tile fromthis room at the desired index

	doorManager.addDoor(&tile, side, roomIndex, floorIndex, isLocked);  //call the addDoor function with all th eparameters ready 
}

void RoomLayouts::addEffect(VE_Type type, DirectionState direction, int floorIndex, int roomIndex, sf::Vector2i mapLocation)
{
	VisualEffect* newEffect = visualEffectManager.addVisualEffect(type, direction, floorIndex, roomIndex);              //add the new effect with all the parameters into the manager
	newEffect->setSize(sf::Vector2f(newEffect->getSize().x * scale, newEffect->getSize().y * scale));                   //scale up its size
	int indexInLevel = (mapLocation.y - 1) * rooms[floorIndex][roomIndex - 1][0].getMapDimensions().x + mapLocation.x;  //get the 1D index from 2D values  
	newEffect->setPosition((*rooms[floorIndex][roomIndex - 1][0].getLevel())[indexInLevel].getPosition());              //set the position to be at the correct tile 
}

Event* RoomLayouts::addEvent(EventType type, int floorIndex, int roomIndex, sf::Vector2i mapLocation)
{
	Event* newEvent = eventManager.addEvent(type, floorIndex, roomIndex);                                               //add the new event with all the parameters into the manager
	int indexInLevel = (mapLocation.y - 1 )* rooms[floorIndex][roomIndex - 1][0].getMapDimensions().x + mapLocation.x;  //get the 1D index from 2D values  
	newEvent->setPosition((*rooms[floorIndex][roomIndex - 1][0].getLevel())[indexInLevel].getPosition());               //set the position to be at the correct tile 
	return newEvent;                                                                                                    //retrun a pointer to this new event, used for further set up
}

void RoomLayouts::addEnemy(EnemyType type, int floorIndex, int roomIndex, sf::Vector2i mapLocation)
{
	Enemy* newEnemy = enemyManager.addEnemy(type, floorIndex, roomIndex);                                               //add the new enemy with all the parameters into the manager  
	int indexInLevel = (mapLocation.y - 1) * rooms[floorIndex][roomIndex - 1][0].getMapDimensions().x + mapLocation.x;  //get the 1D index from 2D values 
	newEnemy->setPosition((*rooms[floorIndex][roomIndex - 1][0].getLevel())[indexInLevel].getPosition());;              //set the position to be at the correct tile
}

void RoomLayouts::renderEvents(sf::RenderWindow* window)
{
	std::vector<Event*> eventsInRoom = eventManager.getEventsInRoom(currentFloor, currentRoom);  //holds all the events inthe current room to reduce access time
	for (int i = 0; i < eventsInRoom.size(); i++)                                                //render all events
	{
		window->draw(*eventsInRoom[i]);
	}
}

void RoomLayouts::renderDoors(sf::RenderWindow* window)
{
	std::vector<Door*> doorsInRoom = doorManager.getDoorsInRoom(currentFloor,currentRoom);    //holds all the doors inthe current room to reduce access time
	for (int i = 0; i < doorsInRoom.size(); i++)                                              //render all events
	{
		window->draw(*doorsInRoom[i]);
	}
}

void RoomLayouts::renderRoom(sf::RenderWindow* window,int layer)
{
	rooms[currentFloor][currentRoom - 1][layer].render(window);            //calls the render function of the current roomMap (specified layer)
}

void RoomLayouts::renderEnemies(sf::RenderWindow* window)
{
	std::vector<Enemy*> enemiesInRoom = enemyManager.getEnemiesInRoom(currentFloor, currentRoom);    //holds all the enemies inthe current room to reduce access time
	for (int i = 0; i < enemiesInRoom.size(); i++)                                                   //render all enemies if alive
	{
		if (enemiesInRoom[i]->isAlive()) {
			window->draw(*enemiesInRoom[i]);
		}
	}
}

void RoomLayouts::renderEffects(sf::RenderWindow* window)
{
	std::vector<VisualEffect*> effectsInRoom = visualEffectManager.getEffectsInRoom(currentFloor, currentRoom);   //holds all the effects inthe current room to reduce access time
	for (int i = 0; i < effectsInRoom.size(); i++)                                                                //render all effects if alive
	{
		if (effectsInRoom[i]->isAlive()) {
			window->draw(*effectsInRoom[i]);
		}
	}
}

void RoomLayouts::collision(Link* player)
{
	
	for (int i = 0; i < rooms[currentFloor][currentRoom - 1].size(); i++) {                                                        // iterate through all the layers
		std::vector<GameObject>* currentRoomLayout = getCurrentRoom(i)->getLevel();                                                //hold the current layer for reduced access time
		for (int j = 0; j < currentRoomLayout->size(); j++) {                                                                      //iterate through all the tiles
			GameObject tile = (*currentRoomLayout)[j];                                                                             //hold the current tile for reduced access time
			if (tile.isCollider() && Collision::checkBoundingBox(&tile, player)) {                                                 //if the tile is a collider and it collides with the player then...  
				player->wallColisionResponse(&tile);                                                                               //...call player wall response
			}
			for (int k = 0; tile.isCollider() && k < enemyManager.getEnemiesInRoom(currentFloor, currentRoom).size()  ; k++) {     //if the tile is a collider, iterate through all th enemies
				Enemy* currentEnemy = enemyManager.getEnemiesInRoom(currentFloor, currentRoom)[k];                                 //hold the current enemy for reduced access time
				if (currentEnemy->isAlive()) {                                                                                     //if the enemy is alive then...
					currentEnemy->setVelocity();                                                                                   //...set its velocity (needs to be here for proper collision response)...
					if (Collision::checkBoundingBox(currentEnemy, &tile)) {                                                        //... and if the enemy ad tile collide then... 
						currentEnemy->wallCollisionResponse(&tile);                                                                //... call enemy wall collision
					}
				}
			}
		}
	}

	doorManager.collision(player);                       // call the collision checks for doors
	eventManager.collision(player);                      // call the collision checks for events 
	enemyManager.collision(player);                      // call th ecollision checks for enemies
}

void RoomLayouts::update(float dt, Link* player)
{
	doorManager.update(dt, sf::Vector2i(currentFloor, currentRoom));             //call the update for doors
	enemyManager.update(dt, sf::Vector2i(currentFloor, currentRoom), player);    //call the update for enemies
	visualEffectManager.update(dt, sf::Vector2i(currentFloor, currentRoom));     //call the update for VEs
}
