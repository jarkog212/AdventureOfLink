#include "Enemy.h"
//base invincibility time for each enemy after hit, in seconds
#define INVINCIBILITY_TIME 0.10f
// how close (in pixels) player needs to be for the enemy to move
#define PLAYER_CLOSENESS 1000

Enemy::Enemy(sf::Texture* T_base, float health, float damageOutput,float enemySpeed, int scale, sf::Vector2i animationCells, sf::Vector2i cellDimensions)
{
	//initialise the pointers
	this->T_base = T_base;
	this->health = health;
	this->damageOutput = damageOutput;
	this->enemySpeed = enemySpeed;
	
	for (int i = 0; i < animationCells.y; i++) {                //create all the animations needed for th enemy, procedurally made
		Animation a;
		a.buildAnimation(cellDimensions, i * cellDimensions.y, animationCells.x, true, animationSpeed);
		A_movements.push_back(a);                               // pushes the animation into the vector 
	}

	setTexture(T_base);
	setSize(sf::Vector2f(cellDimensions * scale));                 //set size to match the texture size * scale
	setCollisionBox(0, getSize().y/2, getSize().x, getSize().y/2); //set collision box to be half of the size, the bottom half, since top-down
	setAlive(true);                                                //enemies start as alive
	stateMachine.setCurrentDirectionState(DirectionState::DOWN);   //set initial direction
	current = &A_movements[0];                                     //set initial animation
}

Enemy::~Enemy()
{
}

void Enemy::update(float dt)
{
	if (invincibilityTime > 0) {                                           //decreases the invincibility time if there is any left
		invincibilityTime -= dt;
	}

	if (abs(velocity.x) > abs(velocity.y)) {                               //based on velocity it infers the direction in which it moves...
		if (A_movements.size() >= 2) {                                     //... and if animation is available it sets it...
			current = &A_movements[1];
		}
		if (velocity.x > 0) {                                              //... as well as flips the texture if necessary...
			current->setFlipped(false);
			stateMachine.setCurrentDirectionState(DirectionState::RIGHT);
		}
		else {
			current->setFlipped(true); 
			stateMachine.setCurrentDirectionState(DirectionState::LEFT);
		}
	}
	else {
		if (velocity.y < 0) {
			if (A_movements.size() >= 3) {                                 //... and if needed/available also changes the animation for upper movement.
				current = &A_movements[2];
			}
			stateMachine.setCurrentDirectionState(DirectionState::UP);
		}
		else {
			current = &A_movements[0];
			stateMachine.setCurrentDirectionState(DirectionState::DOWN);
		}
	}

	current->animate(dt);                                                  // changes/updates the animation
	setTextureRect(current->getCurrentFrame());                            // gets and sets the current texture frame

	velocity *= dt * enemySpeed;                                           // finalises the velocity
	if (isDamageable()) {                                                  // and if is damageable (not in invincibility mode), moves towards the player
		move(velocity);
	}
}

void Enemy::isHit()                          //collision response for a damage
{
	health--;                                //lowers health by one hit
	velocity = sf::Vector2f(0,0);            //stops the enemy from moving
	invincibilityTime = INVINCIBILITY_TIME;  //adds the invincibility time
}

float Enemy::getDamageOutput()
{
	return damageOutput;
}

bool Enemy::isDamageable()
{
	if (invincibilityTime > 0) { return false; }   //if there is any invincibility, the enemy is not damageable 
	return true;
}

void Enemy::setVelocity()
{
	if (Vector::magnitude(desiredSpot - getPosition()) < PLAYER_CLOSENESS) {             // calculates the velocity using the player position if player is close enough
		velocity = (Vector::normalise(desiredSpot - getPosition()));         // normalises th evector (magnitude will be 1)
	}
	else { velocity = sf::Vector2f(0, 0); }                                  // if player is not close then it doesn't move
}

void Enemy::wallCollisionResponse(GameObject* collider)                  
{
	velocity.x *= -1 * 5;                                                            //changes the x and y direction of the enemy
	velocity.y *= -1 * 5;
	if (abs(velocity.x) > abs(velocity.y)) {                                         //infers whether the enemy is moving sideways or vertically, if sideways...
		setPosition(getPosition().x + velocity.x,getPosition().y);                   //...moves the player back in the horizontal plane...
		if (Collision::checkBoundingBox(this, collider)) {                           //...if there is still collision, then...
			setPosition(getPosition().x - velocity.x, getPosition().y + velocity.y); //...moves the player vertically too
		}
	}
	else if (abs(velocity.x) < abs(velocity.y)){                                     //same as above except for sideways it's vertical

		setPosition(getPosition().x, getPosition().y + velocity.y);
		if (Collision::checkBoundingBox(this, collider)) {
			setPosition(getPosition().x + velocity.x, getPosition().y - velocity.y);
		}
	}
	else {
		setPosition(getPosition() + velocity);                                       //if the enemy moved perfectly diagonally, then return the enemy in diagonal line back
	}
}

void Enemy::collisionResponse(GameObject* collider)                                  // empty dummy for special enemies/obstacles to use
{
}
