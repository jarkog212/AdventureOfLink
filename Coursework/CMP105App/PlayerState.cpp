#include "PlayerState.h"

void PlayerState::setCurrentActionState(ActionState s)
{
	currentActionState = s;
}

ActionState PlayerState::getCurrentActionState()
{
	return currentActionState;
}

void PlayerState::setCurrentDirectionState(DirectionState d)
{
	currentDirectionState = d;
}

DirectionState PlayerState::getDirectionState()
{
	return currentDirectionState;
}
