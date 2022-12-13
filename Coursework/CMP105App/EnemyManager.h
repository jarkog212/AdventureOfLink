#pragma once
#include "Enemy_Gooy.h"
#include "Enemy_Gooby.h"
#include "Enemy_Sandy.h"
#include "Enemy_Spike.h"
#include "Enemy_Skeletor.h"
#include "Link.h"
#include "Framework/Collision.h"
#include "FrameWork/AudioManager.h" 

enum class EnemyType { GOOY, GOOBY, SANDY, SPIKE, SKELETOR };

class EnemyManager
{
	std::vector<std::vector<std::vector<Enemy*>>> enemies;     //a 3D vector holding all the enemy object of the game, [floor][room][enemyID]
	sf::Texture T_gooy;                                        //textures for alll the different enemy types
	sf::Texture T_gooby;
	sf::Texture T_sandy;
	sf::Texture T_spike;
	sf::Texture T_skeletor;
	AudioManager* audioManager;                                //enemies do have some sfx hence the pointer to the audio manager 

public:
	EnemyManager() {};                                                          //dummy constructor used to create a dummy container of the EnemyManager class
	EnemyManager(int numOfFloors, int numOfRooms, AudioManager* audioManager);  //main constructor properly initialises this class
	~EnemyManager();                                                            //destructor deletes all the enemies on heap

	Enemy* addEnemy(EnemyType type, int floorIndex, int roomIndex);             //adds the enemy by creating one on the heap and initialises it, returns the pointer to this created enemy
	void update(float dt, sf::Vector2i currentLocation, Link* Player);          //calls the update for the relevant enemies, updates their desired position etc.
	void collision(Link* player);                                               //handless the collision of enemies with attacks and defensless player
	std::vector<Enemy*> getEnemiesInRoom(int currentFloor, int currentRoom) { return enemies[currentFloor][currentRoom - 1]; }    // returns a vector of pointers of enemies in the current room
};

