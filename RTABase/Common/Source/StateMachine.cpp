#include "StateMachine.h"

#include "State.h"

StateMachine::StateMachine( Entity* entity )
{
    this->entity = entity;
    this->current_state = 0;
}

StateMachine::~StateMachine( )
{
}

void StateMachine::Update( float delta_time )
{
    // TODO: Call the current states Execute
	if (current_state != nullptr)
		current_state->Execute(entity, delta_time);
}

void StateMachine::SetEntity( Entity* entity )
{
    this->entity = entity;
}

Entity* StateMachine::GetEntity( )
{
    return entity;
}

void StateMachine::ChangeState( State* state )
{
    // TODO: Change the current state.
    //  Call Exit on the state we are transitioning from ( if there is one ).
    //  Call Enter on the state we are transitioning to.

	if (state != nullptr)
	{
		if (current_state != nullptr)
			current_state->Exit(entity);
		current_state = state;
		current_state->Enter(entity);
	}
}

State* StateMachine::GetCurrentState( )
{
    return current_state;
}