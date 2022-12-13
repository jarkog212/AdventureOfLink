#include "ArrowManager.h"

ArrowManager::ArrowManager()                  // initialises the arrows and laods their texture
{
	T_arrow.loadFromFile("gfx/Arrow.png");    // loads the texture
	for (int i = 0; i < numOfArrows; i++) {   // adds all the arrows on the heap
		quiver[i] = new Arrow(&T_arrow);
	}
}
 
ArrowManager::~ArrowManager()                 // properly frees the memory from itself
{
	for (int i = 0; i < numOfArrows; i++) {   // deletes all the arrow on heap
		delete quiver[i];
	}
}

void ArrowManager::shootArrow(sf::Vector2f playerCenter, sf::Vector2f mousePosition)                 //"shoots the arrow"
{
	
	sf::View view = window->getView();                                                                // gets the view since...
	mousePosition += (view.getCenter()-sf::Vector2f(window->getSize().x/2,window->getSize().y/2));    // the mouse position is relative to the view position (and not the player/world), hence there is a need to offset the mouse position by the position of the view
	sf::Vector2f playerToMouse = mousePosition - playerCenter;                                        // calculate sthe vector for travel of the arrow (difference of mouse position and player position)
	bool flag = false;                                                                                // flag to speed up the iteration through he array
	for (int i = 0; i < numOfArrows && !flag; i++) {                                                  //find and prepare the first arrow that is not ye shot, i.e alive
		if (!quiver[i]->isAlive()) {
			flag = true;
			quiver[i]->prepareArrow(playerToMouse, playerCenter);                                     // calls the preparation function for the arrow, which then shoots the arrow by changing the state  
		}
	}
}

void ArrowManager::update(float dt)                            // calls th eupadte for the relevant arrows
{
	std::vector<Arrow*> shotArrows = getAliveArrows();         // gets all the alive arrows (hence all pof them need to be updated)
	for (int i = 0; i < shotArrows.size(); i++) {              // iterates and update sall the arrows in the returned vector
		shotArrows[i]->update(dt);
	}
}

std::vector<Arrow*> ArrowManager::getAliveArrows()             // gets pointers to all the arrows that are set alive in a vector 
{
	std::vector<Arrow*> aliveArrows;
	for (int i = 0; i < 5; i++) {                              //iterates through all the arrows...
		if (quiver[i]->isAlive()) {                            //... and if alive... 
			aliveArrows.push_back(quiver[i]);                  //... pushes it to the vector
		}
	}
	return aliveArrows;                                        // returns the vector of all the alive arrows
}
