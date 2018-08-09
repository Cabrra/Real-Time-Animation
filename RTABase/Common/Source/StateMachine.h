#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

class Entity;
class State;

class StateMachine
{
public:

    StateMachine( Entity* entity = 0 );
    ~StateMachine( );

public:

    void Update( float delta_time );

    void SetEntity( Entity* entity );
    Entity* GetEntity( );

    void ChangeState( State* state );
    State* GetCurrentState( );

private:

    Entity* entity;
    State* current_state;
};

#endif // _STATE_MACHINE_H_