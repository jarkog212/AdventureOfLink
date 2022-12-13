#pragma once
#include "HUDelement.h"

class HUDe_Heart :
	public HUDelement
{
	float fullness = 1.00f;       // the amount the heart should be filled form 0-1 (in 0.25 increments)

public:
	HUDe_Heart(sf::Texture* texture, Link* player);                 //constructor
	~HUDe_Heart();

	void update(float dt) override;                                 //simple update to reflect the fulness of the heart
	void setFullness(float fullness) { this->fullness = fullness;}  //sets th efullness value
};

