#pragma once
#include "FrameWork/GameObject.h"
#include "Framework/Animation.h"
enum class VE_Type {STREAM_OUT,STREAM_IN,SANDFALL};                       //types of the visal effects (these are really only animated tiles)

class VisualEffect :
	public GameObject
{ 
	Animation* A_base;                                                    //pointer to the animation
	VE_Type type;                                                         //hold the type
	
public:
	VisualEffect(sf::Texture* T_base, Animation* A_base, VE_Type type);   //constructor
	~VisualEffect();
	void update(float dt) override;                                       //override for update
	VE_Type getType() { return type; }
};

