#pragma once
#include "FrameWork\TileMap.h"
class RoomMap :
	public TileMap
{
public:
	RoomMap() {}                                                                              //constructor
	~RoomMap();
	std::vector<int> getTileMap() { return tileMap; }                               
	void changeAllTilesOfType(int typeBefore, int typeAfter);                                 //allows th emodification of the tileMap vector (chnage one type of tiles to another). Chanes all tiles of the type
	void changeAllTilesOfTypes(std::vector<int> typesBefore, int typeAfter);                  //changes multiple types of tiles into one type
	void changeAreaOfTiles(sf::Vector2i initialTileCoords, sf::Vector2i area, int newTile);   //chanes a specified area of tiles into another type (unused)
	sf::Vector2u getMapDimensions() { return mapSize; }   
	void rebuildLevel();                                                                      //needed after tile modification to rebuild the level again so that changes are made
}; 

