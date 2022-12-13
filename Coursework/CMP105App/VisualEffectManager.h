#pragma once
#include "VisualEffect.h"
#include "PlayerState.h"
class VisualEffectManager
{
	std::vector<std::vector<std::vector<VisualEffect*>>> visualEffects;         //3D vector of visual effects [floor][room][effectID]                                              
	sf::Texture T_sandStream;                                                   //texture for sandstream
	sf::Texture T_sandfall;                                                     //texture for sandfall
	Animation A_sandStream[4];                                                  //animations for sandstream
	Animation A_sandfall[4];                                                    //animations for sandfall

public:
	VisualEffectManager(int numOfFloors, int numOfRooms);                                                 //constructor
	~VisualEffectManager();
	void update(float dt,sf::Vector2i currentLocation);                                                   //updates all the animations and calls the VEs updates
	VisualEffect* addVisualEffect(VE_Type type, DirectionState direction, int floorIndex, int roomIndex); //adds the visual effect to the 3d vector and initialises it
	std::vector<VisualEffect*> getEffectsInRoom(int floorIndex, int roomIndex) { return visualEffects[floorIndex][roomIndex - 1]; }
};

