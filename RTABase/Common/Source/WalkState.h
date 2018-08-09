#ifndef _WALK_STATE_H_
#define _WALK_STATE_H_

#include "State.h"

class Entity;

class WalkState : public State
{
private:

    WalkState( );

    // Disable copy constructor and assignment operator
    WalkState( const WalkState& w );
    const WalkState& operator=( const WalkState& w );

public:

    ~WalkState( );

    static WalkState* GetInstance( );

    virtual void Enter( Entity* entity );
    virtual void Execute( Entity* entity, float delta_time );
    virtual void Exit( Entity* entity );
};

#endif // _WALK_STATE_