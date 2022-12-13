#include "Enemy_Spike.h"
#include "Link.h"
//specific texture details for the enemy type
#define HEALTH_HITS 10000
#define DAMAGE_OUTPUT 0.25f
#define SPEED 0.f

#define ANIMATION_FRAMES 1
#define DIRECTIONS 1

#define TEX_CELL_X 18
#define TEX_CELL_Y 18

// just parses the actual values into the base class constructor, the only ones it needs are the texture pointer and scale
Enemy_Spike::Enemy_Spike(sf::Texture* T_base, int scale) : Enemy(T_base, HEALTH_HITS, DAMAGE_OUTPUT, SPEED, scale, sf::Vector2i(ANIMATION_FRAMES, DIRECTIONS), sf::Vector2i(TEX_CELL_X, TEX_CELL_Y))
{
	setCollisionBox(0, 0, getSize().x, getSize().y);
}

Enemy_Spike::~Enemy_Spike()
{
}

void Enemy_Spike::update(float dt)
{
	   // is empty as it doesnt move, essentially a dummy method    
}

void Enemy_Spike::wallCollisionResponse(GameObject* collider)
{
	   // is empty as it should not collide with wall
}

void Enemy_Spike::collisionResponse(GameObject* collider)
{
	Link* player = static_cast<Link*>(collider);   // the check that calls this only happens with the player, hence we cast statically
	player->wallColisionResponse(NULL);            // calls the wall collision response for the player 
}
