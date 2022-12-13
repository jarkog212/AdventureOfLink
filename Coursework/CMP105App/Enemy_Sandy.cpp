#include "Enemy_Sandy.h"
//specific texture details for the enemy type
#define HEALTH_HITS 3
#define DAMAGE_OUTPUT 0.5f
#define SPEED 100.f

#define ANIMATION_FRAMES 2
#define DIRECTIONS 1

#define TEX_CELL_X 41
#define TEX_CELL_Y 16

// just parses the actual values into the base class constructor, the only ones it needs are the texture pointer and scale
Enemy_Sandy::Enemy_Sandy(sf::Texture* T_base, int scale) : Enemy(T_base, HEALTH_HITS, DAMAGE_OUTPUT, SPEED, scale, sf::Vector2i(ANIMATION_FRAMES,DIRECTIONS),sf::Vector2i(TEX_CELL_X,TEX_CELL_Y))
{
}

Enemy_Sandy::~Enemy_Sandy()
{
}
