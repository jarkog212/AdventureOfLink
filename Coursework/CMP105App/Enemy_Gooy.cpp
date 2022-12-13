#include "Enemy_Gooy.h"
//specific texture details for the enemy type
#define HEALTH_HITS 2
#define DAMAGE_OUTPUT 0.25f
#define SPEED 125.f

#define ANIMATION_FRAMES 8
#define DIRECTIONS 1

#define TEX_CELL_X 13
#define TEX_CELL_Y 20

// just parses the actual values into the base class constructor, the only ones it needs are the texture pointer and scale
Enemy_Gooy::Enemy_Gooy(sf::Texture* T_base, int scale) : Enemy(T_base,HEALTH_HITS,DAMAGE_OUTPUT,SPEED,scale,sf::Vector2i(ANIMATION_FRAMES,DIRECTIONS),sf::Vector2i(TEX_CELL_X,TEX_CELL_Y))
{

}

Enemy_Gooy::~Enemy_Gooy()
{
}
