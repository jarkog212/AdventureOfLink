#pragma once
#include "FrameWork/Animation.h"
#include "FrameWork/Vector.h"
#include "PlayerState.h"
#include "FrameWork/GameObject.h"
#include "FrameWork/Collision.h"

class Enemy :
	public GameObject
{
protected:

	PlayerState stateMachine;            // uses the state machne for direction only
	sf::Vector2f desiredSpot;            // essentailly player's position
	std::vector<Animation> A_movements;  // all the animations for enemy (if multiple directions or other applies)
	Animation* current;                  // pointer to the current animation
	sf::Texture* T_base;                 // Texture pointer
	float health;                        // health of the enemy (number of hits)
	float animationSpeed = 0.1f;         // animation speed
	float invincibilityTime = 0;         // time elapsed since hit, used for invincibility frames
	float enemySpeed;                    // actual speed of the enemy
	float damageOutput;                  // damage output of th eenemy (divisible by 0.25f)

public:
	Enemy(sf::Texture* T_base, float health, float damageOutput, float enemySpeed, int scale, sf::Vector2i AnimationCells, sf::Vector2i cellDimensions); // constructor
	~Enemy();

	virtual void update(float dt) override;                                             //virtual function for the enemy ai, can be different for each enemy (sub class)
	virtual void isHit();                                                               // collision response when hot by an attack
	float getDamageOutput();
	bool isDamageable();                                                                // whether the enemy can be damaged, used for invincibility frames
	void setVelocity();                                                                 // set th evelocity with direction towards the player, separate for collision response purposes, i.e. when hit it needs to stop
	float getHealth() { return health; }
	void setDesiredSpot(sf::Vector2f desiredSpot) { this->desiredSpot = desiredSpot; }
	virtual void wallCollisionResponse(GameObject* collider);                           // collision response with the wall, similar to the player, though some thing lik ediagonal movement are take n into consideration
	virtual void collisionResponse(GameObject* collider) override;                      // reserved really for special cases that could be programmed in later

};