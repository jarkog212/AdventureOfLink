#pragma once
#include "Framework/GameObject.h"

class TileSet
{
	float scale = 1;
	std::vector<GameObject> tileSet;                                                                                                   // a vector holding the entire tileset in gameobjects
	sf::Vector2f tileSize;                                                                                                             // tileSize in pixels 
	int clearTile = 1;                                                                                                                 // transparent tile, 1 by default (index 0)
	const char* textureAddress;                                                                                                        // hold the path to the texture file
	sf::Vector2u tileDimensions;                                                                                                       // tile number dimensions

	void initialise(sf::Vector2u tileDimensions, int spacing, const char* textureAddress, sf::Vector2f tileSize, float scale);         //sets of initialisation instruction, used for multiple constructors

public:
	TileSet();                                                                                                                         //dummy constructor
	TileSet(sf::Vector2u tileDimensions, int spacing, const char* textureAddress, sf::Vector2f tileSize, float scale);                 //constructor with default clear tile
	TileSet(sf::Vector2u tileDimensions, int spacing, const char* textureAddress, sf::Vector2f tileSize, float scale, int clearTile);  //constructor with specific clear tile
	~TileSet();

	std::vector<GameObject> getTileSet() { return tileSet; }
	const char* getTexture() { return textureAddress; }
	sf::Vector2u getTileDimensions() { return tileDimensions; }
	sf::Vector2f getTileSize() { return tileSize; }
	float getScale() { return scale; }
	void setCollision(std::vector<int> collisionMap);                                       //based on the input int vector sets the tiles' collision
	void setSemiCollider(int tileNumber) { tileSet[tileNumber].setAlive(true); }            //sets one tile to be a semi collider (i.e collides only with the player and enemy)
};


