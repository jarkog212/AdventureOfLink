#pragma once
#include "Framework/GameObject.h"
#include "RoomMap.h"
#include "DoorManager.h"
#include "EventManager.h"
#include "EnemyManager.h"
#include "VisualEffectManager.h"
#include "TileSet.h"

// class that hold evertything that is room related, enemies, events, doors and visaul effects. Adding new rooms and other objects is simplified using specific methods
class RoomLayouts
{
	sf::Vector2f tileSize = sf::Vector2f(32, 32);                                                                     //base size of a tile (in pixels)
	std::vector<std::vector<std::vector<RoomMap>>> rooms;                                                             //3D vector for tilemaps [floor][room][layer]
	int numOfFloors = 1;                                                                                              //total number of floors
	int numOfRooms = 21;                                                                                              //initial number of rooms per floor 
	int currentRoom = 1;                                                                                              //holds the current room from the player, to reduce access time                                                
	int currentFloor = 0;                                                                                             //holds the current floor from the player, to reduce access time
	int scale = 3;                                                                                                    //tile scale
	TileSet tileSet = TileSet(sf::Vector2u(14, 11), 1, "gfx/Desert_TileSet.png", sf::Vector2f(32, 32), scale, 32);    //tileset to be used, initialised as it can be
	DoorManager doorManager = DoorManager(numOfFloors, numOfRooms, scale);                                            //door manager for the dungeon, can be initialised now
	EventManager eventManager;                                                                                        //dummy placeholder for the actual event manager, cannot be initialised yet as it requires input which will be initialised at the constructor     
	EnemyManager enemyManager;                                                                                        //dummy placeholder for the actual enemy manager, cannot be initialised yet - requires input, player pointers to be initialised
	VisualEffectManager visualEffectManager = VisualEffectManager(numOfFloors, numOfRooms);                           //visual effect manager (glorified animated tiles) can be initialised now 

	void generateDoors(int floor);                                                                                    //procedurally generates doors base on the tileMap
	void createEvents(int floor);                                                                                     //manually added events are cretaed when this is called, list of events is here
	void createEnemies(int floor);                                                                                    //manually added enemies are created when this is called, list of enemies is here
	void createEffects(int floor);                                                                                    //manually added VEs are created when this is called, list of VEs is here  
	void createRoomsF1();                                                                                             //manually added rooms are created when this is called, list of all the room layouts
	void setRoom(std::vector<int>* map, sf::Vector2u mapDimension, std::vector<RoomMap>* storage, TileSet* tileSet);  //set of procedures to properly crate a room
	void setDoors(int floorIndex);                                                                                    //adds aditional setting to the doors (planned to be done on per floor basis)
	void addDoor(int roomIndex, int floorIndex, int tileIndex, DoorSide side, bool isLocked);                         //collects everything to properly initialise the door or parse it further, done to simplify manual addition
	void addEffect(VE_Type type, DirectionState direction, int floorIndex, int roomIndex, sf::Vector2i mapLocation);  //collects everything to properly initialise the effect or parse it further, done to simplify manual addiion
	Event* addEvent(EventType type, int floorIndex, int roomIndex, sf::Vector2i mapLocation);                         //collects everything to properly initialise the event or parse it further, done to simplify manual addition, returns pointer to new event for further setting
	void addEnemy(EnemyType type, int floorIndex, int roomIndex, sf::Vector2i mapLocation);                           //collects everything to properly initialise the enemy or parse it further, done to simplify manual addition

public:
	RoomLayouts(Input* input, AudioManager* audioManager);                                                            //constructor
	~RoomLayouts();
	RoomMap tileMap;                                                                                                  //holder for a roommap
	void update(float dt, Link* player);                                                                              //update function uodates rooms and calls the other updates
	void renderEvents(sf::RenderWindow* window);                                                                      //renders events, separate for layering purposes
	void renderDoors(sf::RenderWindow* window);                                                                       //renders doors, separate for layering purposes
	void renderRoom(sf::RenderWindow* window,int layer);                                                              //renders a specific layer of the room, separate for layering purposes
	void renderEnemies(sf::RenderWindow* window);                                                                     //renders enemies, separate for layering purposes
	void renderEffects(sf::RenderWindow* window);                                                                     //renders effects, separate for layering purposes
	void collision(Link* player);                                                                                     //checks collision with player and rooms as well as calls the other objects collision
	void setCurrentRoom(int currentRoom) { this->currentRoom = currentRoom; }                                         
	void setEventInput(Input* input) { eventManager.setInput(input); }                                                //additionally, if needed can parse the input pointer to the event manager
	int getCurrentRoomNum() { return currentRoom; }                                                                   
	RoomMap* getCurrentRoom(int layer) { return &(rooms[currentFloor][currentRoom-1][layer]); }
	std::vector<Enemy*> getEnemiesInTheRoom() { return enemyManager.getEnemiesInRoom(currentFloor,currentRoom); }     //enemies can be returned further out for arrow collision handled by the level class
};

