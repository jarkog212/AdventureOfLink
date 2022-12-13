#include "TileSet.h"


TileSet::TileSet()
{
}

TileSet::TileSet(sf::Vector2u tileDimensions, int spacing, const char* textureAddress, sf::Vector2f tileSize, float scale)
{
	initialise(tileDimensions, spacing, textureAddress, tileSize, scale);        //parses the parameters into the actual initialiser
}

TileSet::TileSet(sf::Vector2u tileDimensions, int spacing, const char* textureAddress, sf::Vector2f tileSize, float scale, int clearTile)
{
	this->clearTile = clearTile;                                                 //sets the clear tile
	initialise(tileDimensions, spacing, textureAddress, tileSize, scale);        //parses the parameters into the actual initialiser
}

TileSet::~TileSet()
{
}

void TileSet::setCollision(std::vector<int> collisionMap)
{
	for (int i = 0; i < collisionMap.size(); i++) {      //iterates through the whole collisionMap
		if (collisionMap[i] > 0) {                       //if the value is > 0 then...
			tileSet[i].setCollider(true);                //...sets the tile as a collider 
		}
	}
}


void TileSet::initialise(sf::Vector2u tileDimensions, int spacing, const char* textureAddress, sf::Vector2f tileSize, float scale)
{
	//initialises the pointers
	this->tileDimensions = tileDimensions;
	this->tileSize = tileSize;
	this->scale = scale;
	this->textureAddress = textureAddress;
	
	GameObject tile;                                                          //primitive game Object 
	for (int i = 0; i <= tileDimensions.x * tileDimensions.y; i++) {          //ads the proper number of copies into the array, with basic initialisation
		tile.setSize(sf::Vector2f(tileSize.x * scale, tileSize.y * scale));
		tile.setCollisionBox(0, 0, tile.getSize().x, tile.getSize().y);
		tile.setCollider(false);
		tile.setAlive(false);
		tileSet.push_back(tile);
	}

	int posX = 0 + ((int)tileSize.x * ((clearTile - 1) % tileDimensions.x)) + spacing * ((clearTile - 1) % tileDimensions.x);  //gets the position from the clear tile number
	int posY = 0 + ((int)tileSize.y * ((clearTile - 1) / tileDimensions.x)) + spacing * ((clearTile - 1) / tileDimensions.x);
	tileSet[0].setCollider(false);                                                                                             //initialises the clear tile by setting the collider 0 and gets th e correct texture frame
	tileSet[0].setTextureRect(sf::IntRect(posX, posY, tileSize.x, tileSize.y));

	for (int i = 1; i <= tileDimensions.x * tileDimensions.y; i++) {                                              //procedurally creates the rest of the tiles in a sequential order, i.e. easily determinable
		posX = 0 + ((int)tileSize.x * ((i - 1) % tileDimensions.x)) + spacing * ((i - 1) % tileDimensions.x);
		posY = 0 + ((int)tileSize.y * ((i - 1) / tileDimensions.x)) + spacing * ((i - 1) / tileDimensions.x);
		tileSet[i].setTextureRect(sf::IntRect(posX, posY, tileSize.x, tileSize.y));
	}	
}
