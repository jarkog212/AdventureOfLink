#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"
#include "FrameWork/AudioManager.h"
#include "PlayerState.h"
#include "ArrowManager.h"

class Link :
	public GameObject
{
	sf::FloatRect attackCollisionBox;

	Animation A_Base[3];                                 //animations for normal movement (4 directions)
	Animation A_Attack[3];                               //animations for attack (4 directions)
	Animation A_Bow[3];                                  //animations for bow (4 directions)
	Animation* A_current;                                //pointer to the current animation
	sf::Texture T_Link_Base;                             //texture sheet for movement
	sf::Texture T_Link_Attack;                           //texture sheet for attack
	sf::Texture T_Link_Bow;                              //texture sheet for bow
	
	PlayerState stateMachine;                            //state machine used for both action and direction
	AudioManager* audioManager;                          //audio manage rpointer as th eplayer does play sounds
	ArrowManager arrowManager;                           //arrow manager for the arrows that the playershoots

	float moveSpeed = 400;                               //player movement speed
	sf::Vector2f centerPoint;                            //center point of the player, calculated by halving the size
	sf::Vector2f viewOffset = sf::Vector2f(0.0f,0.0f);   //view offset, used to move the view pixels from the player centre, used for animation purposes as cetian textures need the offsetting
	bool finishedAttack = true;                          //whther the player finished attacking
	int currentRoom = 1;                                 //current room where the player is in
	int currentFloor = 0;                                //current floor where the player is in
	float animationSpeed = 0.05f;                        //animation speed base 
	int scale = 5;                                       //scale base
	float walkSFXdelay = 0.22f;                          //how much time in seconds needs to elapse before the walk sfx plays again
	float walkingTime = 0.f;                             //used for sfx playing purposes
	float invincibilityTime = 0;                         //the time remaining for the invincibility, used for invincibility frames
	int dungeonKeys = 0;                                 //the number of keys the player has 
	float health = 4.0f;                                 //the actual health player has, will be only in the multiples of 0.25
	

	void setAttackBoundingBoxes();                    //sets up the attack collision boxes as they shift around based on the state and direction
	void setAttackAnimationOffset(bool begining);     //offsets the camera and player position to accomodate in shifts in animation textures
	void transitionAttack_Base();                     //resets everything properly after the attack to change the state back to base

public:
	Link(AudioManager* audioManager);                                                                                                      //constructor
	~Link();
	
	void setAttackCollisionBox(float x, float y, float width, float height) { attackCollisionBox = sf::FloatRect(x, y, width, height); }
	float getHealth() { return health; }
	void giveDamage(float damageTaken);                                                                                                    //decreases the health by the parameter
	DirectionState getDirection() { return stateMachine.getDirectionState(); }
	ActionState getAction() { return stateMachine.getCurrentActionState(); } 
	sf::Vector2i getCurrentLocation() { return sf::Vector2i(currentFloor,currentRoom); }                                                   //location as in floor and room
	void setCurrentLocation(int currentFloor, int currentRoom) { this->currentRoom = currentRoom; this->currentFloor = currentFloor; }
	int getDungeonKeys() { return dungeonKeys; }
	void addDungeonKey() { dungeonKeys++; }
	void useDungeonKey() { dungeonKeys--; }
	bool isDamageable();                                                                                                                   //whether the player can be damaged, used for invincibility frames
	void handleInput(float dt) override;                                                                                                   //handles the input for the player
	void update(float dt) override;                                                                                                        //updates the player
	sf::FloatRect getAttackCollisionBox() { return sf::FloatRect(attackCollisionBox.left + getPosition().x, attackCollisionBox.top + getPosition().y, attackCollisionBox.width, attackCollisionBox.height); }
	void wallColisionResponse(GameObject* collider);                                                                                       //collision response to the wall, uses direction
	sf::Vector2f getCenterPoint() { return centerPoint; }
	void updateCenterPoint();                                                                                                              //sets the center point based on the size, which changes in accordance with the texture/animation in use
	std::vector<Arrow*> getAliveArrows() { return arrowManager.getAliveArrows(); }                                                         //parses a call to the arrow manager and parses the return back
	void parseWindow(sf::RenderWindow* window) { arrowManager.setWindow(window); }                                                         //parses the window to the arrow manager
	
};

