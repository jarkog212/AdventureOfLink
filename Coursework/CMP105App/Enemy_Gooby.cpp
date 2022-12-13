#include "Enemy_Gooby.h"
//specific texture details for the enemy type
#define HEALTH_HITS 1
#define DAMAGE_OUTPUT 0.25f
#define SPEED 300.f

#define ANIMATION_FRAMES 2
#define DIRECTIONS 3

#define TEX_CELL_X 16
#define TEX_CELL_Y 16

// just parses the actual values into the base class constructor, the only ones it needs are the texture pointer and scale
Enemy_Gooby::Enemy_Gooby(sf::Texture* T_base, int scale) : Enemy(T_base, HEALTH_HITS, DAMAGE_OUTPUT, SPEED, scale, sf::Vector2i(ANIMATION_FRAMES, DIRECTIONS), sf::Vector2i(TEX_CELL_X, TEX_CELL_Y))
{
}

Enemy_Gooby::~Enemy_Gooby()
{
}
