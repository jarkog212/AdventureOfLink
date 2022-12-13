#include "EnemyManager.h"
#define SCALE 5

EnemyManager::EnemyManager(int numOfFloors, int numOfRooms, AudioManager* audioManager)
{
	//initialises the pointers
	this->audioManager = audioManager;
	//loads the textures
	T_gooby.loadFromFile("gfx/Enemy_Gooby.png");
	T_gooy.loadFromFile("gfx/Enemy_Gooy.png");
	T_sandy.loadFromFile("gfx/Enemy_Sandy.png");
	T_skeletor.loadFromFile("gfx/Enemy_Skeletor.png");
	T_spike.loadFromFile("gfx/Enemy_Spike.png");

	for (int i = 0; i < numOfFloors; i++) {   // properly initialises the 3D vector

		std::vector<std::vector<Enemy*>> s;
		enemies.push_back(s);

		for (int j = 0; j < numOfRooms; j++) {
			std::vector<Enemy*> d;
			enemies[i].push_back(d);
		}
	}
}

EnemyManager::~EnemyManager()
{
	// iterates through the vector to delete all the enemies on heap
	for (int i = 0; i < enemies.size(); i++) {
		for (int j = 0; j < enemies[i].size(); j++) {
			for (int k = 0; k < enemies[i][j].size(); k++)
			{
				delete enemies[i][j][k];                                     //deletes the enemy on heap
			}
			enemies[i][j].erase(enemies[i][j].begin(),enemies[i][j].end());  //deletes the empty vector
		}
		enemies[i].erase(enemies[i].begin(), enemies[i].end());              //deletes the empty vector
	}
	enemies.erase(enemies.begin(), enemies.end());                           //deletes the empty vector
}

Enemy* EnemyManager::addEnemy(EnemyType type, int floorIndex, int roomIndex) //adds the enemy of the type into the proper vector room
{
	//creates all the types of enemies on the heap
	Enemy_Gooby* gooby = new Enemy_Gooby(&T_gooby, SCALE);
	Enemy_Gooy* gooy = new Enemy_Gooy(&T_gooy, SCALE);
	Enemy_Sandy* sandy = new Enemy_Sandy(&T_sandy, SCALE);
	Enemy_Skeletor* skeletor = new Enemy_Skeletor(&T_skeletor, SCALE);
	Enemy_Spike* spike = new Enemy_Spike(&T_spike, SCALE);

	//switch based on the desired type of te enemy
	switch (type) {
	case EnemyType::GOOBY:  
		delete gooy;                                           //deletes the rest of the types
		delete sandy;
		delete skeletor;
		delete spike;
		enemies[floorIndex][roomIndex - 1].push_back(gooby);   //adds the desired one into the vector
		return gooby;                                          //returns the pointer to it
		break;
	case EnemyType::GOOY:
		delete sandy;                                          //same as above case
		delete skeletor;
		delete spike;
		delete gooby;
		enemies[floorIndex][roomIndex - 1].push_back(gooy);
		return gooy;
		break;
	case EnemyType::SANDY:
		delete gooby;                                          //same as above case
		delete gooy;
		delete skeletor;
		delete spike;
		enemies[floorIndex][roomIndex - 1].push_back(sandy);
		return sandy;
		break;
	case EnemyType::SKELETOR:
		delete gooby;                                          //same as above case
		delete gooy;
		delete spike;
		delete sandy;
		enemies[floorIndex][roomIndex - 1].push_back(skeletor);
		return skeletor;
		break;
	case EnemyType::SPIKE:
		delete gooby;                                          //same as above case
		delete gooy;
		delete sandy;
		delete skeletor;
		enemies[floorIndex][roomIndex - 1].push_back(spike);
		return spike;
		break;
	}
}

void EnemyManager::update(float dt, sf::Vector2i currentLocation, Link* player)               // updates only the relevant enemies
{
	for (int i = 0; i < getEnemiesInRoom(currentLocation.x, currentLocation.y).size(); i++) { //iterates through all the enmies in the rom

		if (getEnemiesInRoom(currentLocation.x, currentLocation.y)[i]->isAlive()) {           //if the enmy is alive...
			Enemy* currentEnemy = getEnemiesInRoom(currentLocation.x, currentLocation.y)[i];  //...hold a pointer to the enemy, reduce access time...
			currentEnemy->setDesiredSpot(player->getPosition());                              //... set player's position as the enemy's desired position...
			currentEnemy->update(dt);                                                         //... call its update
			if (currentEnemy->getHealth() <= 0) {                                             // if the enemy's health is below or equal 0...
				currentEnemy->setAlive(false);                                                //...set it dead         
			}
		}
	}
}

void EnemyManager::collision(Link* player)                                                    //collision handler method
{
	for (int i = 0; i < getEnemiesInRoom(player->getCurrentLocation().x, player->getCurrentLocation().y).size(); i++)                                                      // iterates through all the enemies in the room
	{
		Enemy* currentEnemy = getEnemiesInRoom(player->getCurrentLocation().x, player->getCurrentLocation().y)[i];                                                         // save the current enemy's address to reduce access time
		if (currentEnemy->isAlive()) {                                                                                                                                     // if the enemy is alive...
			if (Collision::checkBoundingBox(currentEnemy, player))                                                                                                         // ... and collides with the defensless player...
			{
				currentEnemy->collisionResponse(player);                                                                                                                   // ...call enemy's standard non-damaging collision response
				if (player->isDamageable()) {                                                                                                                              // if the player is damageable...
					player->giveDamage(currentEnemy->getDamageOutput());                                                                                                   // ... call player's damage collision response...
					audioManager->playSoundbyName("playerHitSFX");                                                                                                         // ... and play sfx
				}
				
			}
			if (currentEnemy->isDamageable() && player->getAction() == ActionState::ATTACK && Collision::checkBoundingBox(player->getAttackCollisionBox(), currentEnemy))  //if enemy is damageable and player is attacking and enemy collides with the attack collision box...
			{
				currentEnemy->isHit();                                                                                                                                     //...call enemy's damaged collision box...
				audioManager->playSoundbyName("enemyHitSFX");                                                                                                              //... and play sfx
			}
		}
	}
}
