#include "HUDe_WeaponType.h"
#define TEX_CELL_SIZE_X 18
#define TEX_CELL_SIZE_Y 18
#define TEX_SCALE 3.5f

HUDe_WeaponType::HUDe_WeaponType(sf::Texture* texture, Link* player) :                          //asks for parameters that are variable...
	HUDelement(texture, player, sf::Vector2i(TEX_CELL_SIZE_X, TEX_CELL_SIZE_Y), 2, TEX_SCALE)   //...as some of them are constant per type and can be filled in
{
}

HUDe_WeaponType::~HUDe_WeaponType()
{
}

void HUDe_WeaponType::update(float dt)
{
	ActionState playerAction = player->getAction();           // uses theb player pointer to get the current action state, stores ot to reduce access time
	switch (playerAction) {                                  
	//switch based on the action state
	case ActionState::BOW:
		setTextureRect(states[1]);  //case bow chanes the texture rectangle storing the frame for bow
		break;
	default:
		setTextureRect(states[0]);  //default is sword
		break;
	}
}
