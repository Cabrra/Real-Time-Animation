#ifndef _STATE_H_
#define _STATE_H_

class Entity;

class State
{
public:

    State( );
    virtual ~State( );

    virtual void Enter( Entity* entity ) = 0;
    virtual void Execute( Entity* entity, float delta_time ) = 0;
    virtual void Exit( Entity* entity ) = 0;
};

#endif // _STATE_H_