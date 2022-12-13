#include "VisualEffectManager.h"
//texture specific constants
#define TEX_SANDSTREAM_CELL_X 64
#define TEX_SANDSTREAM_CELL_Y 64
#define SANDSTREAM_FRAMES 9
#define SANDSTREAM_SPEED 0.05f

#define TEX_SANDFALL_CELL_X 32
#define TEX_SANDFALL_CELL_Y 32
#define SANDFALL_FRAMES 2
#define SANDFALL_SPEED 0.25f

VisualEffectManager::VisualEffectManager(int numOfFloors, int numOfRooms)
{
	//load the textures
	T_sandStream.loadFromFile("gfx/VE_Sandstream.png");
	T_sandfall.loadFromFile("gfx/VE_SandstreamEnd.png");
	//build the animations
	A_sandStream[0].buildAnimation(sf::Vector2i(TEX_SANDSTREAM_CELL_X, TEX_SANDSTREAM_CELL_Y), 0, SANDSTREAM_FRAMES, true, SANDSTREAM_SPEED);
	A_sandStream[1].buildAnimation(sf::Vector2i(TEX_SANDSTREAM_CELL_X, TEX_SANDSTREAM_CELL_Y), TEX_SANDSTREAM_CELL_Y, SANDSTREAM_FRAMES, true, SANDSTREAM_SPEED);
	A_sandStream[2].buildAnimation(sf::Vector2i(TEX_SANDSTREAM_CELL_X, TEX_SANDSTREAM_CELL_Y), TEX_SANDSTREAM_CELL_Y, SANDSTREAM_FRAMES, true, SANDSTREAM_SPEED);
	A_sandStream[2].setFlipped(true);
	A_sandStream[3].buildAnimation(sf::Vector2i(TEX_SANDSTREAM_CELL_X, TEX_SANDSTREAM_CELL_Y), TEX_SANDSTREAM_CELL_Y*2, SANDSTREAM_FRAMES, true, SANDSTREAM_SPEED);

	A_sandfall[0].buildAnimation(sf::Vector2i(TEX_SANDFALL_CELL_X, TEX_SANDFALL_CELL_Y), 0, SANDFALL_FRAMES, true, SANDFALL_SPEED);
	A_sandfall[1].buildAnimation(sf::Vector2i(TEX_SANDFALL_CELL_X, TEX_SANDFALL_CELL_Y), TEX_SANDFALL_CELL_Y, SANDFALL_FRAMES, true, SANDFALL_SPEED);
	A_sandfall[2].buildAnimation(sf::Vector2i(TEX_SANDFALL_CELL_X, TEX_SANDFALL_CELL_Y), TEX_SANDFALL_CELL_Y, SANDFALL_FRAMES, true, SANDFALL_SPEED);
	A_sandfall[2].setFlipped(true);
	A_sandfall[3].buildAnimation(sf::Vector2i(TEX_SANDFALL_CELL_X, TEX_SANDFALL_CELL_Y), TEX_SANDFALL_CELL_Y*2, SANDFALL_FRAMES, true, SANDFALL_SPEED);
	
	

	for (int i = 0; i < numOfFloors; i++) {        //initialise the 3d vector

		std::vector<std::vector<VisualEffect*>> s;
		visualEffects.push_back(s);

		for (int j = 0; j < numOfRooms; j++) {
			std::vector<VisualEffect*> d;
			visualEffects[i].push_back(d);
		}
	}
}

VisualEffectManager::~VisualEffectManager()
{
	for (int i = 0; i < visualEffects.size(); i++) {
		for (int j = 0; j < visualEffects[i].size(); j++) {
			for (int k = 0; k < visualEffects[i][j].size(); k++)
			{
				delete visualEffects[i][j][k];                                                  //delete the effect on heap
			}
			visualEffects[i][j].erase(visualEffects[i][j].begin(), visualEffects[i][j].end());  //delete the empty vector
		}
		visualEffects[i].erase(visualEffects[i].begin(), visualEffects[i].end());               //delete the empty vector
	}
	visualEffects.erase(visualEffects.begin(), visualEffects.end());                            //delete the empty vector
}

void VisualEffectManager::update(float dt,sf::Vector2i currentLocation)
{
	for (int i = 0; i < 4; i++) {                                                               //call all animations update/animate
		A_sandfall[i].animate(dt);
		A_sandStream[i].animate(dt);
	}
	for (int i = 0; i < visualEffects[currentLocation.x][currentLocation.y - 1].size(); i++) {  //call all current room's VEs updates
		visualEffects[currentLocation.x][currentLocation.y - 1][i]->update(dt);
	}
}

VisualEffect* VisualEffectManager::addVisualEffect(VE_Type type, DirectionState direction, int floorIndex, int roomIndex)
{
	VisualEffect* newEffect;                                                                //creates a pointer to be filled
	//switch based on type, diverges into the switch of the direction and based on that creates a new effect on heap, with proper initialisation
	switch (type) {
	case VE_Type::STREAM_IN: 
	case VE_Type::STREAM_OUT:
		switch (direction) {
		case DirectionState::DOWN:
			newEffect = new VisualEffect(&T_sandStream, &A_sandStream[0], type);
			break;
		case DirectionState::RIGHT:
			newEffect = new VisualEffect(&T_sandStream, &A_sandStream[1], type);
			break;
		case DirectionState::LEFT:
			newEffect = new VisualEffect(&T_sandStream, &A_sandStream[2], type);
			break;
		default://case DirectionState::UP:
			newEffect = new VisualEffect(&T_sandStream, &A_sandStream[3], type);
			break;
		}
		newEffect->setSize(sf::Vector2f(TEX_SANDSTREAM_CELL_X, TEX_SANDSTREAM_CELL_Y));    //sets size to Match the texture 
		break;
	case VE_Type::SANDFALL:
		switch (direction) {
		case DirectionState::DOWN:
			newEffect = new VisualEffect(&T_sandfall, &A_sandfall[0], type);
			break;
		case DirectionState::RIGHT:
			newEffect = new VisualEffect(&T_sandfall, &A_sandfall[1], type);
			break;
		case DirectionState::LEFT:
			newEffect = new VisualEffect(&T_sandfall, &A_sandfall[2], type);
			break;
		default://case DirectionState::UP:
			newEffect = new VisualEffect(&T_sandfall, &A_sandfall[3], type);
			break;
		}
		newEffect->setSize(sf::Vector2f(TEX_SANDFALL_CELL_X, TEX_SANDFALL_CELL_Y));        //sets size to Match the texture 
		break;
	}
	visualEffects[floorIndex][roomIndex - 1].push_back(newEffect);                         //pushes the effect into the proper place in the 3D array 
	return newEffect;                                                                      //returns th epointer to the effect for further setting
}
 