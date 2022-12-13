#include "VisualEffect.h"

VisualEffect::VisualEffect(sf::Texture* T_base, Animation* A_base, VE_Type type)
{
	//initialises the pointers
	this->type = type;
	this->A_base = A_base;
	//initialises this obect
	setAlive(false);
	setCollider(false);
	setTexture(T_base);
}

VisualEffect::~VisualEffect()
{
}

void VisualEffect::update(float dt)
{
	setTextureRect(A_base->getCurrentFrame());     //since the animation is hanfdled by the manager, this only takes the current frame from the pointer
}
