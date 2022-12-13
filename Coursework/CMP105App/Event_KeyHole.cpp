#include "Event_KeyHole.h"

Event_KeyHole::~Event_KeyHole()
{
}

void Event_KeyHole::collisionResponse(GameObject* collider)
{
	if (input->isKeyDown(sf::Keyboard::LShift)) {       //if interaction button is pressed 
		
		Link* player = static_cast<Link*>(collider);    // since the interaction happens with the player alone, we can cast statically
		
		if (linkedDoor->unlock(player)) {               //unlocks tries to unlock the door using the player's keys it retruns true when it succeeds...  
			interactable = false;                       //... disables interactibility...
			setTextureRect(stateInactive);              //... changes texture frame...
			audioManager->playSoundbyName("secretSFX"); //... plays sfx
		}
	}
}
