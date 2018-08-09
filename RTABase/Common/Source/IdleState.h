#ifndef _IDLE_STATE_H_
#define _IDLE_STATE_H_

#include "State.h"

class IdleState : public State
{
private:

    IdleState( );

    // Disable copy constructor and assignment operator
    IdleState( const IdleState& i );
    const IdleState& operator=( const IdleState& i );

public:

    ~IdleState( );

    static IdleState* GetInstance( );

    virtual void Enter( Entity* entity );
    virtual void Execute( Entity* entity, float delta_time );
    virtual void Exit( Entity* entity );
};

#endif // _IDLE_STATE_H_