#include "Link.h"
#include <iostream>
// texture specific constants
#define ATTACK_DOWN_CELL_X 24
#define ATTACK_DOWN_CELL_Y 31
#define ATTACK_SIDE_CELL_X 31
#define ATTACK_SIDE_CELL_Y 22
#define ATTACK_UP_CELL_X 25
#define ATTACK_UP_CELL_Y 34

#define BASE_UNI_CELL_X 16
#define BASE_UNI_CELL_Y 23

#define BOW_UNI_CELL_X 20
#define BOW_UNI_CELL_Y 24
// the number of seconds to be invincible after being hit
#define INVINCIBILITY_TIME 1.f

Link::Link(AudioManager* audioManager)
{
	// initialise th epointer
	this->audioManager = audioManager;
	
	//build the animations
	A_Attack[0].buildAnimation(sf::Vector2i(ATTACK_DOWN_CELL_X, ATTACK_DOWN_CELL_Y), 0, 7, false, animationSpeed / 2);
	A_Attack[1].buildAnimation(sf::Vector2i(ATTACK_SIDE_CELL_X, ATTACK_SIDE_CELL_Y),ATTACK_DOWN_CELL_Y, 7, false, animationSpeed / 2);
	A_Attack[2].buildAnimation(sf::Vector2i(ATTACK_UP_CELL_X, ATTACK_UP_CELL_Y), ATTACK_DOWN_CELL_Y + ATTACK_SIDE_CELL_Y, 7, false, animationSpeed / 2);
	for (int i = 0; i < 3; i++) {
		A_Base[i].buildAnimation(sf::Vector2i(BASE_UNI_CELL_X, BASE_UNI_CELL_Y), i * BASE_UNI_CELL_Y, 11, true, animationSpeed);
		A_Bow[i].buildAnimation(sf::Vector2i(BOW_UNI_CELL_X, BOW_UNI_CELL_Y), i * BOW_UNI_CELL_Y, 1, true, 0.0f);
	}

	setSize(sf::Vector2f(BASE_UNI_CELL_X*scale,BASE_UNI_CELL_Y*scale));                                      //set the size to match the textre frame

	//load the textures
	T_Link_Attack.loadFromFile("gfx/Link_Attack.png");
	T_Link_Base.loadFromFile("gfx/Link_Base.png");
	T_Link_Bow.loadFromFile("gfx/Link_Bow.png");

	setTexture(&T_Link_Base);                                                                                //set the base texture

	setCollider(true);                                                                                       //player is always a collider
	setCollisionBox(0, BASE_UNI_CELL_Y * scale / 2, BASE_UNI_CELL_X * scale, BASE_UNI_CELL_Y * scale / 2);   //set the collision box to match the texture
	setAttackCollisionBox(0, 0, 0, 0);                                                                       //create a dummy attack collision box

	//set the default state to direction DOWN and action BASE
	stateMachine.setCurrentDirectionState(DirectionState::DOWN);
	stateMachine.setCurrentActionState(ActionState::BASE);

	A_current = &A_Base[0];                        //set the current animation
	A_current->animate(0);                         //just get the first frame...
	setTextureRect(A_current->getCurrentFrame());  //...and set it as the texture frame
}

Link::~Link()
{
}

void Link::giveDamage(float damageTaken)    
{  
	health -= damageTaken;                     //dicreases health
	invincibilityTime = INVINCIBILITY_TIME;    //gives invincibility time
}

bool Link::isDamageable()
{
	if (invincibilityTime > 0) { return false; }     //if there is any invincibility time left the player is not damageable...
	return true;                                     //...otherwise they are
}

void Link::handleInput(float dt)
{
	velocity = sf::Vector2f(0.0f, 0.0f);                                               //reset the velocity
	if (finishedAttack && stateMachine.getCurrentActionState() != ActionState::BOW) {  //if the attack is finished (or not happening) and the action stae is not bow...
		if (!(stateMachine.getCurrentActionState() == ActionState::BASE) &&            //...if action state is not base and...
			!(input->isKeyDown(sf::Keyboard::Up)) &&                                   //...no input is pressed then...
			!(input->isKeyDown(sf::Keyboard::Down)) &&
			!(input->isKeyDown(sf::Keyboard::Left)) &&
			!(input->isKeyDown(sf::Keyboard::Right)))
		{
			stateMachine.setCurrentActionState(ActionState::BASE);                     //...set the state to be base
			return;
		}
		if (input->isKeyDown(sf::Keyboard::Up)) {                                      //if direction up is pressed...
			if (stateMachine.getCurrentActionState() == ActionState::BASE ||           //...and if state machine is base or already up (prevents changing the direction without first going to base) then...
				stateMachine.getDirectionState() == DirectionState::UP)
			{
				stateMachine.setCurrentActionState(ActionState::MOVE);                 //... set the state to move and direction to up...
				stateMachine.setCurrentDirectionState(DirectionState::UP);
				velocity += sf::Vector2f(0.0f, -moveSpeed);                            //... change the velocity to go up
			}
			else { stateMachine.setCurrentActionState(ActionState::BASE); }            //... if the input up is pressed but the states aren't matching then just set the state to base
		}
		else if (input->isKeyDown(sf::Keyboard::Right)) {                              //same as above with a different direction
			if (stateMachine.getCurrentActionState() == ActionState::BASE ||
				stateMachine.getDirectionState() == DirectionState::RIGHT)
			{
				stateMachine.setCurrentActionState(ActionState::MOVE);
				stateMachine.setCurrentDirectionState(DirectionState::RIGHT);
				velocity += sf::Vector2f(moveSpeed, 0.0f);
			}
			else { stateMachine.setCurrentActionState(ActionState::BASE); }
		}
		else if (input->isKeyDown(sf::Keyboard::Down)) {                               //same as above with a different direction
			if (stateMachine.getCurrentActionState() == ActionState::BASE ||
				stateMachine.getDirectionState() == DirectionState::DOWN)
			{
				stateMachine.setCurrentActionState(ActionState::MOVE);
				stateMachine.setCurrentDirectionState(DirectionState::DOWN);
				velocity += sf::Vector2f(0.0f, moveSpeed);
			}
			else { stateMachine.setCurrentActionState(ActionState::BASE); }
		}
		else if (input->isKeyDown(sf::Keyboard::Left)) {                               //same as above with a different direction
			if (stateMachine.getCurrentActionState() == ActionState::BASE ||
				stateMachine.getDirectionState() == DirectionState::LEFT)
			{
				stateMachine.setCurrentActionState(ActionState::MOVE);
				stateMachine.setCurrentDirectionState(DirectionState::LEFT);
				velocity += sf::Vector2f(-moveSpeed, 0.0f);
			}
			else { stateMachine.setCurrentActionState(ActionState::BASE); }
		}
		if (input->isKeyDown(sf::Keyboard::Space)) {                                   // if the attack is pressed then...
			stateMachine.setCurrentActionState(ActionState::ATTACK);                   // ... set the action to attack...
			audioManager->playSoundbyName("swordSFX");                                 // ... play sfx
		}
	}
	 
	if (input->isKeyDown(sf::Keyboard::LControl) && stateMachine.getCurrentActionState() == ActionState::BASE)        // if mode switch is pressed and action is base then...
	{
		input->setKeyUp(sf::Keyboard::LControl); 
		stateMachine.setCurrentActionState(ActionState::BOW);                                                         // ...change the action to bow...
		audioManager->playSoundbyName("weaponSwitchSFX");                                                             // ...play sfx
	}
	else if (input->isKeyDown(sf::Keyboard::LControl) && stateMachine.getCurrentActionState() == ActionState::BOW) {  // same as above but reverse switching
		input->setKeyUp(sf::Keyboard::LControl);
		stateMachine.setCurrentActionState(ActionState::BASE);                                     
		audioManager->playSoundbyName("weaponSwitchSFX");
	}
}

void Link::update(float dt)
{
	arrowManager.update(dt);                                                      // call update for arrows
	
	if (invincibilityTime > 0) {                                                  // if ther eis any invincibility time, decrease it
		invincibilityTime -= dt; }

	//masive switch base on the action
	switch (stateMachine.getCurrentActionState()) {
		case ActionState::BASE:                                                   //case action is base...

			setSize(sf::Vector2f(BASE_UNI_CELL_X*scale,BASE_UNI_CELL_Y*scale));   //set the size to match the base texture
			setTexture(&T_Link_Base);                                             //set the texture to base

			//switch based on direction
			switch (stateMachine.getDirectionState()) {                         
			case DirectionState::DOWN:                                            //case down
				A_current = &A_Base[0];                                           //set the appropriate animation
				break;
			case DirectionState::LEFT:                                            //same logic as previous case
				A_current = &A_Base[1];
				A_current->setFlipped(true);
				break;
			case DirectionState::RIGHT:                                           //same logic as previous case
				A_current = &A_Base[1];
				A_current->setFlipped(false);
				break;
			case DirectionState::UP:                                              //same logic as previous case
				A_current = &A_Base[2];
				break;
			}
			A_current->reset();                                                   //reset the animation back to te first frame
			break;

		case ActionState::MOVE:                                                   // case action is move

			setTexture(&T_Link_Base);                                             // set the texture to base (is also for move)
			setSize(sf::Vector2f(BASE_UNI_CELL_X*scale, BASE_UNI_CELL_Y*scale));  // set the size to match the texture frame size * scale
			walkingTime += dt;                                                    // add to the walking time, used for sfx

			if (walkingTime >= walkSFXdelay) {                                    //if the walking time is larger that the refressh time for sfx, play sfx and reset walking time
				audioManager->playSoundbyName("walkSFX");
				walkingTime = 0;
			}

			//switch based on the direction, where proper animation is chosen based on the direction
			switch (stateMachine.getDirectionState()) {
				case DirectionState::DOWN:
					A_current = &A_Base[0]; 
					break;
				case DirectionState::LEFT:
					A_current = &A_Base[1];
					A_current->setFlipped(true);
					break;
				case DirectionState::RIGHT:
					A_current = &A_Base[1];
					A_current->setFlipped(false);
					break;
				case DirectionState::UP:
					A_current = &A_Base[2];
					break;
			}
			break;

		case ActionState::ATTACK:                                                              //case action is attack

			if (finishedAttack) {                                                              //if the attack was finished before (has not been happening yet) then...
				setAttackAnimationOffset(true);                                                //...set the attack animation offset to match the attack...
				setAttackBoundingBoxes();                                                      //...set the correct collision boxes and shift them around...
				finishedAttack = false;                                                        //...sets the attack as not yet finished...
				setTexture(&T_Link_Attack);                                                    //...sets the texture for attack
			}

			//switch based on the direction, sets the correct size to match the correct texture and sets the animation
			switch (stateMachine.getDirectionState()) {
				case DirectionState::DOWN:
					setSize(sf::Vector2f(ATTACK_DOWN_CELL_X*scale, ATTACK_DOWN_CELL_Y*scale));
					A_current = &A_Attack[0];
					break;
				case DirectionState::LEFT:
					setSize(sf::Vector2f(ATTACK_SIDE_CELL_X*scale, ATTACK_SIDE_CELL_Y*scale));
					A_current = &A_Attack[1];
					A_current->setFlipped(true);
					break;
				case DirectionState::RIGHT:
					setSize(sf::Vector2f(ATTACK_SIDE_CELL_X*scale, ATTACK_SIDE_CELL_Y*scale));
					A_current = &A_Attack[1];
					A_current->setFlipped(false);
					break;
				case DirectionState::UP:
					setSize(sf::Vector2f(ATTACK_UP_CELL_X*scale, ATTACK_UP_CELL_Y*scale));
					A_current = &A_Attack[2];
					break;
			}
			break;

		case ActionState::BOW:                                                              //case action is bow
			setTexture(&T_Link_Bow);                                                        //set the texture 
			setSize(sf::Vector2f(BOW_UNI_CELL_X * scale, BOW_UNI_CELL_Y * scale));          //set the size to match the texture * scale
			
			//switch based on direction, sets the correct animation
			switch (stateMachine.getDirectionState()) {
			case DirectionState::DOWN:
				A_current = &A_Bow[0];
				break;
			case DirectionState::LEFT:
				A_current = &A_Bow[1];
				A_current->setFlipped(true);
				break;
			case DirectionState::RIGHT:
				A_current = &A_Bow[1];
				A_current->setFlipped(false);
				break;
			case DirectionState::UP:
				A_current = &A_Bow[2];
				break;
			}
			if (input->isMouseLDown()) {                                                                                                                                               //if shoot button is pressed...
				input->setMouseLDown(false);                   
				arrowManager.shootArrow(sf::Vector2f(getPosition().x + getSize().x / 2, getPosition().y + getSize().y / 2), sf::Vector2f(input->getMouseX(), input->getMouseY()));     //...shoot the arrow (delegating and setting up th earrow object)...
				audioManager->playSoundbyName("bowSFX");                                                                                                                               //...play sfx
			}
			break;
	}

	if (stateMachine.getCurrentActionState() == ActionState::ATTACK && !A_current->getPlaying()) {      //if the action is attack and animation is no longer playing then...
		transitionAttack_Base();                                                                        //...transition back to base
	} 

 	A_current->animate(dt);                         //animate the current animation
	setTextureRect(A_current->getCurrentFrame());   //get the frame and set it as texture frame

	updateCenterPoint();                            //update the players center point, world location

	velocity *= dt;                                 //make velocity on per frame basis
	move(velocity);                                 //move the player
}

void Link::wallColisionResponse(GameObject* collider)     // collision response to the wall
{
	//switch based on the direction, move player back by 10 ixels in the opposite direction and reset the velocity if the player continues to try to move in the direction
	switch (stateMachine.getDirectionState()) {           
		case DirectionState::DOWN:
			if (velocity.y > 0) {
				velocity = sf::Vector2f(0, 0);
			}
			setPosition(sf::Vector2f(getPosition().x, getPosition().y - 10));
			break;
		case DirectionState::LEFT:
			if (velocity.x < 0) {
				velocity = sf::Vector2f(0, 0);
			}
			setPosition(sf::Vector2f(getPosition().x + 10, getPosition().y));
			break;
		case DirectionState::RIGHT:
			if (velocity.x > 0) {
				velocity = sf::Vector2f(0, 0);
			}
			setPosition(sf::Vector2f(getPosition().x - 10, getPosition().y));
			break;
		case DirectionState::UP:
			if (velocity.y < 0) {
				velocity = sf::Vector2f(0, 0);
			}
			setPosition(sf::Vector2f(getPosition().x, getPosition().y + 10));
			break;	
	}
}

void Link::updateCenterPoint()
{
	centerPoint = sf::Vector2f(getPosition().x + BASE_UNI_CELL_X*scale / 2, getPosition().y + BASE_UNI_CELL_Y*scale / 2);   //get the center point by getting the current position and adding the half of the size to it
	centerPoint += viewOffset;                                                                                              //also adds any offset that migh be nedded for the view, if 0,0 then this is harmless
}

void Link::setAttackAnimationOffset(bool begining)
{
	if (begining) {                                                     // if we just begin the attack then...
		//...switch based on the direction, set the correct position offset and view offset based on that position offset
		switch (stateMachine.getDirectionState()) {                     
		case DirectionState::DOWN:
			setPosition(getPosition() - sf::Vector2f(8*scale,0));
			viewOffset = sf::Vector2f(8*scale,0);
			break;
		case DirectionState::LEFT:
			setPosition(getPosition() - sf::Vector2f(15*scale, 0));
			viewOffset = sf::Vector2f(15 * scale, 0);
			break;
		case DirectionState::UP:
			setPosition(getPosition() - sf::Vector2f(0, 10 * scale));
			viewOffset = sf::Vector2f(0,10 * scale);
			break;
		}
	}
	else {                                                              //otherwise...
		//..switch base don direction, reset the position offset back... 
		 
		switch (stateMachine.getDirectionState()) {
		case DirectionState::DOWN:
			setPosition(getPosition() + sf::Vector2f(8*scale, 0));
			break;
		case DirectionState::LEFT:
			setPosition(getPosition() + sf::Vector2f(15*scale, 0));
			break;
		case DirectionState::UP:
			setPosition(getPosition() + sf::Vector2f(0, 10*scale));
			break;
		}
		viewOffset = sf::Vector2f(0, 0);                                //... and reset the view offset              
	}
}

void Link::transitionAttack_Base()
{
	finishedAttack = true;                                                                           //set attack as finished
	A_current->setPlaying(true);                                                                     //reset the animation to be playing for next attack
	A_current->reset();                                                             
	setAttackAnimationOffset(false);                                                                 //restore the offsets back
	setCollisionBox(0, BASE_UNI_CELL_Y*scale / 2, BASE_UNI_CELL_X*scale, BASE_UNI_CELL_Y*scale / 2); //restore the standard collision box
	setAttackCollisionBox(0, 0, 0, 0);                                                               //get rid of the attack collision box
	stateMachine.setCurrentActionState(ActionState::BASE);                                           //set the action to base
}

void Link::setAttackBoundingBoxes()
{
	//switch based on the direction, sets the attack collision box at the correct place/size and shifts around the standard collision box
	switch (stateMachine.getDirectionState()) {
		case DirectionState::DOWN:
			setCollisionBox((ATTACK_DOWN_CELL_X - BASE_UNI_CELL_X)*scale, BASE_UNI_CELL_Y*scale/2, BASE_UNI_CELL_X*scale, BASE_UNI_CELL_Y*scale/2);
			setAttackCollisionBox((ATTACK_DOWN_CELL_X - BASE_UNI_CELL_X) * scale, BASE_UNI_CELL_Y*scale, BASE_UNI_CELL_X*scale, (ATTACK_DOWN_CELL_Y - BASE_UNI_CELL_Y)*scale);
			break;
		case DirectionState::RIGHT:
			setCollisionBox(0, BASE_UNI_CELL_Y*scale/2, BASE_UNI_CELL_X * scale, BASE_UNI_CELL_Y * scale/2);
			setAttackCollisionBox(BASE_UNI_CELL_X*scale, 0 , (ATTACK_SIDE_CELL_X - BASE_UNI_CELL_X)*scale, BASE_UNI_CELL_Y*scale);
			break;
		case DirectionState::LEFT:
			setCollisionBox((ATTACK_SIDE_CELL_X - BASE_UNI_CELL_X) * scale, BASE_UNI_CELL_Y * scale / 2, BASE_UNI_CELL_X * scale, BASE_UNI_CELL_Y * scale / 2);
			setAttackCollisionBox(0, 0, (ATTACK_SIDE_CELL_X - BASE_UNI_CELL_X) * scale, BASE_UNI_CELL_Y * scale);
			break;
		case DirectionState::UP:
			setCollisionBox(0,(ATTACK_UP_CELL_Y - BASE_UNI_CELL_Y/2)*scale, BASE_UNI_CELL_X*scale, BASE_UNI_CELL_Y * scale / 2);
			setAttackCollisionBox(0, 0, BASE_UNI_CELL_X * scale, (ATTACK_DOWN_CELL_Y - BASE_UNI_CELL_Y/2) * scale);
		}
}
