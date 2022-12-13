#include "Event_Chest.h"

Event_Chest::~Event_Chest()
{
}

void Event_Chest::collisionResponse(GameObject* collider)
{
	Link* player = static_cast<Link*>(collider);        // collsiion interaction needs the player anyway so we know the collider is the player, hence the static cast

	if (input->isKeyDown(sf::Keyboard::LShift)) {       //if interaction button is pressed...
		player->addDungeonKey();                        //...give player the dungeon key...
		interactable = false;                           //... set as non-interactable...
		setTextureRect(stateInactive);                  //... change texture ...
		audioManager->playSoundbyName("chestOpenSFX");  //... play sfx
	}
}
