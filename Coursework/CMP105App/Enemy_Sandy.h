#pragma once
#include "Enemy.h"
class Enemy_Sandy :
	public Enemy
{
public:
	Enemy_Sandy(sf::Texture* T_base, int scale); //most of the enemy constructor parameters are constants for each type hence only the ones tha are not are needed
	~Enemy_Sandy();
};

