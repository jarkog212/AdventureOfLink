#include "RoomMap.h"

RoomMap::~RoomMap()
{
}

void RoomMap::changeAllTilesOfType(int typeBefore, int typeAfter)
{
	for (int i = 0; i < tileMap.size(); i++) {                                                    //searches through the entire vector
		if (tileMap[i] == typeBefore) { tileMap[i] = typeAfter; }                                 //if it finds the type to be changed, changes it to new type
	}
	rebuildLevel();                                                                               //rebuilds level to apply changes
}

void RoomMap::changeAllTilesOfTypes(std::vector<int> typesBefore, int typeAfter)
{
	for (int i = 0; i < typesBefore.size(); i++) {                                                //iterates through the whole list of tile types to be changed
		for (int j = 0; j < tileMap.size(); j++) {                                                //searches through the whole vector fot each tile type
			if (tileMap[j] == typesBefore[i]) { tileMap[j] = typeAfter; }                         //if it finds the type to be changed, changes it to new type
		}
	} 
	rebuildLevel();                                                                               //rebuilds level to apply changes
}

void RoomMap::changeAreaOfTiles(sf::Vector2i initialTileCoords, sf::Vector2i area, int newTile)
{
	int tileIndex = initialTileCoords.x * initialTileCoords.y - 1;               //starts at th einitial tile
	int tilesToChange = area.x * area.y;                                         //calculates how many tiles it still neds to change (i.e. how many more iterations of the loop)
	for (int i = 1; i <= tilesToChange; i++)                                     //iterates until no more changes 
	{ 
		tileMap[tileIndex] = newTile;                                            //sets the old tile to new value
		if (i % area.x == 0) { tileIndex += mapSize.x; }                         //if at the end of the row of the specified area, moves onto the next row
		else { tileIndex++; }                                                    //otherwise just ads 1 to the next tile
	}
	rebuildLevel();                                                              //rebuilds level to apply changes
}

void RoomMap::rebuildLevel()
{
	std::vector<GameObject> s;                                                   //creates a new empty vector
	level = s;                                                                   //replaces the old level
	buildLevel();                                                                //builds the new level
}
