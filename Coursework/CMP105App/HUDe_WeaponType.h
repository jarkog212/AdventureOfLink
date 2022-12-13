#pragma once
#include "HUDelement.h"
#include "Link.h"

class HUDe_WeaponType :
	public HUDelement
{

public:
	HUDe_WeaponType(sf::Texture* texture, Link* player);   // constructor
	~HUDe_WeaponType(); 
	void update(float dt) override;                        //update based on the player's action state (attack mode)
};

