#include "Arrow.h"
// defines all the texture pixels to be easily changable if the texture for arrow changes  
#define TEX_DIR1_X 5
#define TEX_DIR1_Y 0
#define TEX_DIR2_X 10
#define TEX_DIR2_Y 1
#define TEX_DIR3_X 62
#define TEX_DIR3_Y 5
#define TEX_DIR4_X 36
#define TEX_DIR4_Y 1
#define TEX_DIR5_X 0
#define TEX_DIR5_Y 0
#define TEX_DIR6_X 23
#define TEX_DIR6_Y 1
#define TEX_DIR7_X 62
#define TEX_DIR7_Y 0
#define TEX_DIR8_X 49
#define TEX_DIR8_Y 1
// defines the texture heights and widths 
#define ARROW_STRAIGHT_HEIGHT 15
#define ARROW_STRAIGHT_WIDTH 5
#define ARROW_DIAGONAL_HEIGHT 14
#define ARROW_DIAGONAL_WIDTH 13
// defines the scale
#define SCALE 5

Arrow::Arrow(sf::Texture* T_base)
{
	setTexture(T_base);                                                                                    // sets the texture from the pointer
	setAlive(false);                                                                                       // sets the object as not alive as it is alive only when shot

	texturePieces[0] = sf::IntRect(TEX_DIR1_X,TEX_DIR1_Y,ARROW_STRAIGHT_WIDTH,ARROW_STRAIGHT_HEIGHT);      // sets all the texture rectangles for all the 8 direction
	texturePieces[1] = sf::IntRect(TEX_DIR2_X, TEX_DIR2_Y, ARROW_DIAGONAL_WIDTH, ARROW_DIAGONAL_HEIGHT);
	texturePieces[2] = sf::IntRect(TEX_DIR3_X, TEX_DIR3_Y, ARROW_STRAIGHT_HEIGHT, ARROW_STRAIGHT_WIDTH);
	texturePieces[3] = sf::IntRect(TEX_DIR4_X, TEX_DIR4_Y, ARROW_DIAGONAL_WIDTH, ARROW_DIAGONAL_HEIGHT);
	texturePieces[4] = sf::IntRect(TEX_DIR5_X, TEX_DIR5_Y, ARROW_STRAIGHT_WIDTH, ARROW_STRAIGHT_HEIGHT);
	texturePieces[5] = sf::IntRect(TEX_DIR6_X, TEX_DIR6_Y, ARROW_DIAGONAL_WIDTH, ARROW_DIAGONAL_HEIGHT);
	texturePieces[6] = sf::IntRect(TEX_DIR7_X, TEX_DIR7_Y, ARROW_STRAIGHT_HEIGHT, ARROW_STRAIGHT_WIDTH);
	texturePieces[7] = sf::IntRect(TEX_DIR8_X, TEX_DIR8_Y, ARROW_DIAGONAL_WIDTH, ARROW_DIAGONAL_HEIGHT);
}

Arrow::~Arrow()
{
}

void Arrow::update(float dt)
{
	if (isAlive() && isShot) {         //this means that the arrows is shot and did not hit anything yet, i.e. is still "flying"
		elapsedTime += dt;             // adds to the elapsed time
		move(velocity * dt);           // moves the arrow further
		if (elapsedTime > 2) {         // if the elapsed time is more than 2 seconds...
			setAlive(false);           //...gets rid of the arrow
			isShot = false;
			elapsedTime = 0;           
		}
	} 
	else if (isAlive()) {              //if its alive but hit the wall, it no longer collides with objects and just remains shown without moving
		elapsedTime += dt;             // adds to the elapsed time
		if (elapsedTime > 1) {         // if its more than one second, i.e. is stuck in wall for more than 1 second...
			setAlive(false);           //...gets rid of the arrow
			elapsedTime = 0;
		}
	}
}

void Arrow::wallCollisionResponse()    // when the arrow hits wall
{
	setCollider(false);         // changes the arrow to no longer be a collider
	isShot = false;             // changes the state of the arrow
	elapsedTime = 0;            // resets the elapsed time
}

void Arrow::collisionResponse(GameObject* collider)   // collision with the enemy
{
	isShot = false;             //changes the state of the arrow
	setAlive(false);            //gets rid of the arrow
}

void Arrow::prepareArrow(sf::Vector2f mouseToPlayer, sf::Vector2f playerPosition)   // all the preparations that is needed before shooting the arrow
{

	mouseToPlayer /= Vector::magnitude(mouseToPlayer);							//normalises (changes the magnitude to 1) the voctor from the player to the mouse cursor 
	velocity = mouseToPlayer * arrowSpeed;                                      //multiplies the normalised vector by the arrow speed 

	int angle;                                                                  // will hold the angle value
	if (velocity.y < 0) {                                                       // if the velocity is less than 0, then the arrow will shoot to the north of the player
		if (velocity.x > 0) {                                                   // if the velocity is more than 0, then the arrow will shoot to the north-east
			angle = (atan(abs(velocity.y) / velocity.x) * 180) / 3.14;          // since we are we know which quadrant the arrow will be shot to, we can calculate the angle form the X axis (arctan(y/x)*(180/PI) = angle (in DEG))
			if (angle > 68) {                                                   // the entire circle is divided into 8 (360/8 = 45) and the shift of all the angles is bu half the smallest piece (45/2 = 22.5), this makes each quadran hold 3 directions. 0-22,22-68,68-90
				setTextureRect(texturePieces[0]);                               // changes the texture to the appropriate direction
			}
			else if (angle > 22) {
				setTextureRect(texturePieces[1]);
			}
			else {
				setTextureRect(texturePieces[2]);
			}
		}
		else {                                                                  // arrow will be shot in west-north quadrant
			angle = (atan(abs(velocity.y) / abs(velocity.x)) * 180) / 3.14;
			if (angle > 68) {
				setTextureRect(texturePieces[0]);
			}
			else if (angle > 22) {
				setTextureRect(texturePieces[7]);
			}
			else {
				setTextureRect(texturePieces[6]);
			}
		}
	}
	else {                                                                      // arrow will be shot to south
		if (velocity.x > 0) {                                                   // arrow will be shot to south - east
			angle = (atan(velocity.y / velocity.x) * 180) / 3.14;
			if (angle > 68) {
				setTextureRect(texturePieces[4]);
			}
			else if (angle > 22) {
				setTextureRect(texturePieces[3]);
			}
			else {
				setTextureRect(texturePieces[2]);
			}
		}
		else {                                                                  // arrow will be shot to south - west
			angle = (atan(velocity.y / abs(velocity.x)) * 180) / 3.14;
			if (angle > 68) {
				setTextureRect(texturePieces[4]);
			}
			else if (angle > 22) {
				setTextureRect(texturePieces[5]);
			}
			else {
				setTextureRect(texturePieces[6]);
			}
		}
	}
	
	setSize(sf::Vector2f(getTextureRect().width*SCALE, getTextureRect().height*SCALE));   // sets the size to match the texture rect
	setPosition(playerPosition.x-getSize().x/2, playerPosition.y-getSize().y/2);          // sets the initial position to the cente of the player  
	setCollisionBox(0, 0, getSize().x, getSize().y);                                      // sets the collision box to match the size
	isShot = true;                                                                        // changes the state of the arrow
	setAlive(true);                                                                       // makes it redereable
	setCollider(true);                                                                    // enables its collision
}
