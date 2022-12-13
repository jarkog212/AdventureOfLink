#include "Event_FlowSwitch.h"

Event_FlowSwitch::~Event_FlowSwitch()
{
}

void Event_FlowSwitch::collisionResponse(GameObject* collider)       // fills in the sand in the linked room and enables visual effects 
{
	if (input->isKeyDown(sf::Keyboard::LShift)) {                                                                  //if interaction button is pressed...
			
		linkedRoom->changeAllTilesOfTypes(std::vector<int> {22,153}, 80);                                          //...replace all tileMap tiles 22 and 153 (chasms) with 80 (sand)...
		linkedRoom->changeAllTilesOfTypes(std::vector<int> {7, 8, 9, 21, 23, 35, 36, 37}, 31);                     //...replace all tilemap tiles (nubers in the param are edge tiles) with 31 (standard tile)...

		for (int i = 0; i < originRoomEffects.size(); i++) {                                                       //iterate throuh all visual effects in the origin room...
			VisualEffect* currentEffect = originRoomEffects[i];                                                    //...store the current effect to reduce access time...
			if (currentEffect->getType() == VE_Type::STREAM_OUT) {                                                 //...aand if its the effect of sand going out of the room...
				currentEffect->setAlive(true);                                                                     //...set it rendereable
			}
		}

		for (int i = 0; i < linkedRoomEffects.size(); i++) {                                                       //..same as above except sets the sand comin in the room and falls as rendereable in the linked room
			VisualEffect* currentEffect = linkedRoomEffects[i];
			if (currentEffect->getType() == VE_Type::STREAM_IN || currentEffect->getType() == VE_Type::SANDFALL) {
				currentEffect->setAlive(true);
			}
		}

		interactable = false;                          //disables interactibility
		setTextureRect(stateInactive);                 //changes the texture frame
		audioManager->playSoundbyName("sandfallSFX");  //plays sounds
		audioManager->playSoundbyName("secretSFX");
	}
}

void Event_FlowSwitch::setLinkedRoom(RoomMap* linkedRoom, std::vector<VisualEffect*> linkedRoomEffects, std::vector<VisualEffect*> originRoomEffects)
{
	//initialise the pointers
	this->linkedRoom = linkedRoom;
	this->linkedRoomEffects = linkedRoomEffects;
	this->originRoomEffects = originRoomEffects;
}
