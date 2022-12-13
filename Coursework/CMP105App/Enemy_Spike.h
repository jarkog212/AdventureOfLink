#pragma once
#include "Enemy.h"
class Enemy_Spike :
	public Enemy
{
public:
	Enemy_Spike(sf::Texture* T_base, int scale);                  //most of the enemy constructor parameters are constants for each type hence only the ones tha are not are needed
	~Enemy_Spike();
	void update(float dt) override;                               //has specific behaviour, i.e. dummy
	void wallCollisionResponse(GameObject* collider) override;    //acts more like a wall, hence should not collide with the wall i.e. a dummy
	void collisionResponse(GameObject* collider) override;        //calls player wall collision response
};

