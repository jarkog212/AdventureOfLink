#include "HUD.h"
#include <string>
//HUD specific setting for proper placemetns and visibility of the elements on the screen
#define HEARTS_SPACING 10
#define BAR_OPACITY 0.75f
#define BAR_TEX_X 500
#define BAR_TEX_Y 56
#define BAR_SCALE 1.75f 
#define KEY_SCALE 5
#define KEY_TEX_X 13
#define KEY_TEX_Y 12
#define KEY_CELL_N 5

#define POS_RELATIVE_HEARTS_X 30
#define POS_RELATIVE_Y 20
#define POS_RELATIVE_KEY_X 460
#define POS_RELATIVE_WEAPON_X 725

HUD::HUD(sf::RenderWindow* window, Link* player)
{
	//initialises the pointers
	this->window = window;
	this->player = player;
	// loads the textures
	T_heartsAndKey.loadFromFile("gfx/HUD_StatusHearts.png");
	T_weaponType.loadFromFile("gfx/HUD_WeaponStates.png");
	T_bar.loadFromFile("gfx/HUD_Bar.png");
	//initialises the bar
	bar.setFillColor(sf::Color(255, 255, 255, 255 * BAR_OPACITY));
	bar.setSize(sf::Vector2f(BAR_TEX_X*BAR_SCALE,BAR_TEX_Y*BAR_SCALE));
	bar.setTexture(&T_bar);
	//initialises the key object
	key.setSize(sf::Vector2f(KEY_TEX_X * KEY_SCALE, KEY_TEX_Y * KEY_SCALE));
	key.setTexture(&T_heartsAndKey);
	key.setTextureRect(sf::IntRect(KEY_TEX_X * (KEY_CELL_N - 1), 0, KEY_TEX_X, KEY_TEX_Y));
	// initialises the text
	keyNumber.setString("x " + std::to_string(player->getDungeonKeys()));
	keyNumber.setCharacterSize(50);

	numOfHearts = 4;

	for (int i = 0; i < numOfHearts; i++) {                     //add all the heart object copies into the vector
		HUDe_Heart s = HUDe_Heart(&T_heartsAndKey, player);
		hearts.push_back(s);
	}
	// creates the Hud element for weapons on heap, uses player to get the weapon state within update
	weaponType = new HUDe_WeaponType(&T_weaponType, player);

}

HUD::~HUD()
{
	delete weaponType;  // deletes the element on heap
}

void HUD::renderHUD()   //renders the hud in proper order
{
	window->draw(bar);                       //renders bar...
	for (int i = 0; i < numOfHearts; i++) {  //...then all hearts... 
		window->draw(hearts[i]);             
	}  
	window->draw(key);                       //...then key...
	window->draw(keyNumber);                 //...then number of keys...
	window->draw(*weaponType);               //...lastly weapon type
}
 
void HUD::update()      // updates the HUD
{
	setPosition();      //sets the new position based on the view

	keyNumber.setString("x " + std::to_string(player->getDungeonKeys()));  //updates the number of keys from the player
	weaponType->update(0);                                                 //updates the weapon type
	updateHealth();                                                        //updates the health
}

void HUD::updateHealth()  //transforms the health value from the player into heart system
{
	float currentHealth = player->getHealth();     // stores health for reduced access time
	int i = 0;                                     // initialises the iterative i now, for scope acess purposes
	for (i; i < player->getHealth()-1; i++) {      // iterates all full hearts (i.e. all full int points of health)
		hearts[i].setFullness(1.00f);              // set full heart
		hearts[i].update(0);                       // update the heart element so it reflects the fulness
		currentHealth--;                           // decreases the remaining health to be used after all full hearts are rendered
	}
	if (i-1 < numOfHearts) {                       //since i is left at 1 higher than the last index dcrease by one and check whether there could be more hearts if so...
		hearts[i].setFullness(currentHealth);      //... set the fulness of the last heart the remaining health...
		hearts[i].update(0);                       //... update to reflect fulness 
		i++;                                       // increase i
		for (i; i < numOfHearts; i++) {            // if thee are still hearts keft iterate throug them and...
			hearts[i].setFullness(0.00f);          // ...set their fulness to 0...
			hearts[i].update(0);                   // ...update to reflect fulness
		}
	}
}

void HUD::setPosition()    // takes the view position and window size to determine the proper hud position
{
	sf::Vector2f origin = sf::Vector2f(window->getView().getCenter().x - floor(window->getSize().x / 2), window->getView().getCenter().y - floor(window->getSize().y/2)); //calculate the origin (top left corner) from relative view to the world coordinates
	
	bar.setPosition(origin);                                                                     //set the bar position to the origin
	
	for (int i = 0; i < hearts.size(); i++) {                                                    //iterate through all the hearts
		
		int newX = origin.x + POS_RELATIVE_HEARTS_X * (i + 1) + hearts[0].getSize().x * i;       //and set their position relative to eachother and the origin
		hearts[i].setPosition(newX,origin.y + POS_RELATIVE_Y);
	}
	 
	key.setPosition(origin.x + POS_RELATIVE_KEY_X,origin.y + POS_RELATIVE_Y);                    // set the key position relative to the Origin
	keyNumber.setPosition(key.getPosition().x + key.getSize().x + 20,origin.y + POS_RELATIVE_Y); // set the key number position relative to the key
	weaponType->setPosition(origin.x + POS_RELATIVE_WEAPON_X,origin.y + POS_RELATIVE_Y);         // set the weapon type position relative to the origin

}