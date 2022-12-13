#include "Screen_MenuMap.h"
//texture specific constans
#define LINKICON_CELL_X 16
#define LINKICON_CELL_Y 14
#define LINKICON_SCALE 5

#define MAPF1_CELL_X 507
#define MAPF1_CELL_Y 404
#define MAPF1_OFFSET 20
#define MAPF1_SCALE 2.0f
//text specific constants
#define TEXT_OFFSET_X 15
#define TEXT_OFFSET_Y 80

Screen_MenuMap::Screen_MenuMap(sf::RenderWindow* window, Input* input, Link* player, GameState* gameState, AudioManager* audioManager,sf::Font* F_base, sf::Texture* T_background):
	Screen(window,input,gameState,audioManager,F_base,T_background)    //parses the parameters into th ebase constructor
{
	//initialises the pointer
	this->player = player;

	//initialises the vector
	std::vector<sf::Vector2i> s;                                                                   
	relativeIconLocations.push_back(s);

	//loads th etextures
	T_MapF1.loadFromFile("gfx/Map_LV1.png");
	T_LinkIcon.loadFromFile("gfx/LinkIcon.png");

	//initialise the text
	text.setCharacterSize(60);
	text.setFillColor(sf::Color::Black);

	//initialise the objects
	F1Map.setTexture(&T_MapF1);
	F1Map.setSize(sf::Vector2f(MAPF1_CELL_X * MAPF1_SCALE, MAPF1_CELL_Y * MAPF1_SCALE));
	
	linkIcon.setTexture(&T_LinkIcon);
	linkIcon.setSize(sf::Vector2f(LINKICON_CELL_X * LINKICON_SCALE, LINKICON_CELL_Y * LINKICON_SCALE));
	
	setRelativeIconLocations();
	setPosition();
}

Screen_MenuMap::~Screen_MenuMap()
{
}

void Screen_MenuMap::setRelativeIconLocations()
{
	// manually defined icon positions for each room
	//F1 R1
	relativeIconLocations[0].push_back(sf::Vector2i(250, 386));
	//F1 R2
	relativeIconLocations[0].push_back(sf::Vector2i(249, 333));
	//F1 R3
	relativeIconLocations[0].push_back(sf::Vector2i(249, 280));
	//F1 R4
	relativeIconLocations[0].push_back(sf::Vector2i(366, 272));
	//F1 R5
	relativeIconLocations[0].push_back(sf::Vector2i(433, 203));
	//F1 R6
	relativeIconLocations[0].push_back(sf::Vector2i(384, 202));
	//F1 R7
	relativeIconLocations[0].push_back(sf::Vector2i(340, 217));
	//F1 R8
	relativeIconLocations[0].push_back(sf::Vector2i(422, 155));
	//F1 R9
	relativeIconLocations[0].push_back(sf::Vector2i(423, 111));
	//F1 R10
	relativeIconLocations[0].push_back(sf::Vector2i(341, 131));
	//F1 R11
	relativeIconLocations[0].push_back(sf::Vector2i(249, 124));
	//F1 R12
	relativeIconLocations[0].push_back(sf::Vector2i(250, 240));
	//F1 R13
	relativeIconLocations[0].push_back(sf::Vector2i(169, 227));
	//F1 R14
	relativeIconLocations[0].push_back(sf::Vector2i(186, 160));
	//F1 R15
	relativeIconLocations[0].push_back(sf::Vector2i(125, 131));
	//F1 R16
	relativeIconLocations[0].push_back(sf::Vector2i(27, 141));
	//F1 R17
	relativeIconLocations[0].push_back(sf::Vector2i(75, 177));
	//F1 R18
	relativeIconLocations[0].push_back(sf::Vector2i(60, 256));
	//F1 R19
	relativeIconLocations[0].push_back(sf::Vector2i(158, 272));
	//F1 R20
	relativeIconLocations[0].push_back(sf::Vector2i(250, 63));
	//F1 R21
	relativeIconLocations[0].push_back(sf::Vector2i(250, 19));
}

void Screen_MenuMap::handleInput()
{
	if (input->isKeyDown(sf::Keyboard::Enter)) {                                         //if menu button is pressed then...
		input->setKeyUp(sf::Keyboard::Enter);
		gameState->setCurrentState(State::LEVEL);                                        //...switch state to level...
		audioManager->playSoundbyName("menuButtonSFX");                                  //... play sfx...
		audioManager->getMusic()->setVolume(audioManager->getMusic()->getVolume() * 2);  //... raising the volume up back
	}
}

void Screen_MenuMap::setPosition()
{
	sf::Vector2f origin = sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2, window->getView().getCenter().y - window->getSize().y / 2);             //gets the current origin, based on the view center

	background.setPosition(origin.x, origin.y);                                                                                                                           //sets the position of background based on the origin
	F1Map.setPosition(window->getView().getCenter().x - F1Map.getSize().x/2, window->getView().getCenter().y - F1Map.getSize().y / 2);                                    //sets the position of map object to the center of the view
	text.setPosition(origin.x + TEXT_OFFSET_X, origin.y + TEXT_OFFSET_Y);                                                                                                 //sets the text position relative to the origin
	float iconX = F1Map.getPosition().x + relativeIconLocations[player->getCurrentLocation().x][player->getCurrentLocation().y-1].x*MAPF1_SCALE - linkIcon.getSize().x/2; //calculates proper link icon position
	float iconY = F1Map.getPosition().y + relativeIconLocations[player->getCurrentLocation().x][player->getCurrentLocation().y-1].y*MAPF1_SCALE - linkIcon.getSize().y/2;
	linkIcon.setPosition(iconX, iconY);
}

void Screen_MenuMap::screenUpdate(float dt)
{
	setPosition();                                                                   //sets the position
	text.setString("Floor " + std::to_string(player->getCurrentLocation().x+1));     //update sthe string (current floor)
}

void Screen_MenuMap::render()
{
	//renders all the objects
	beginDraw();
	window->draw(background);
	window->draw(F1Map);
	window->draw(text);
	window->draw(linkIcon);
	endDraw();
}

