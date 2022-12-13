#pragma once
#include "FrameWork/GameObject.h"
#include "FrameWork/Vector.h"
// class for a singular arrow object most of the functionality and collision responses are here. Objects oh this class are handled by the ArrowManager
class Arrow :
	public GameObject
{
	float elapsedTime = 0;          //used for dissapearing after some time either after stuck on wall or after just being shot
	sf::IntRect texturePieces[8];   //8 texture parts are for 8 directions, while the arrow can be shot in all 360 degs, this is then divided into 8 parts 
	bool isShot = false;            // a state of the arrow 
	float arrowSpeed = 1200.0f;     // the arrow speed per second

public:
	Arrow(sf::Texture* T_base);     //constructor
	~Arrow();

	void update(float dt) override;                                              // moves the arrow at a velocity if the arrow is shot
	void wallCollisionResponse();                                                // when collides with the wall, gets stuck for a short time and then dissapears
	void collisionResponse(GameObject* collider) override;                       // enemy collision response, gives damage and dissapears immediately
	void prepareArrow(sf::Vector2f mouseToPlayer, sf::Vector2f playerPosition);  // sets the direction and chooses the texture (and size of the objects) before being shot
};

