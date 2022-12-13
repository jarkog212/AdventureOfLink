#pragma once
enum class ActionState { BASE, ATTACK, MOVE, BOW };     //actions the player can do 
enum class DirectionState { UP, DOWN, LEFT, RIGHT };    //direction the player can be in

class PlayerState
{
public:

	void setCurrentActionState(ActionState s);          
	ActionState getCurrentActionState();
	void setCurrentDirectionState(DirectionState d);
	DirectionState getDirectionState();

protected:
	ActionState currentActionState;                     //stores the action state
	DirectionState currentDirectionState;               //stores the direction state
};

