#include "HUDe_Heart.h"
// texture specific constants
#define TEX_CELL_X 13
#define TEX_CELL_Y 12
#define HEARTS_TEX_SCALE 5

HUDe_Heart::HUDe_Heart(sf::Texture* texture, Link* player) :                                //asks for parameters that are variable...
	HUDelement(texture, player, sf::Vector2i(TEX_CELL_X, TEX_CELL_Y), 4, HEARTS_TEX_SCALE)  //...as some of them are constant per type and can be filled in
{
}

HUDe_Heart::~HUDe_Heart()
{
}

void HUDe_Heart::update(float dt)
{
	if (fullness == 0) {                                      //if the fullness is 0...
		setFillColor(C_transparent);                          //...just makes the object dissapear
	}
	else {                                                    
		setFillColor(C_opaque);                               //...otherwise makes the object visible... 
		setTextureRect(states[(int)abs((fullness * 4) - 4)]); //...sets the proper texture for heart fullness, for example   0.25*4 = 1 - 4 = -3 abs 3, 3 is the index for a texture of a heart filled with 1/4 
	}
	
}
