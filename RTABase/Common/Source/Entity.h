#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "StateMachine.h"
#include "AnimationBlender.h"
#include "matrix4.h"

class Mesh;

template < typename T >
class Animation;

class Entity
{
    friend class State;

public:

    Entity( );
    ~Entity( );

    void Update( float delta_time );

    void SetMesh( Mesh* mesh );
    Mesh* GetMesh( );

    StateMachine& GetStateMachine( );
    AnimationBlender< matrix4f >& GetAnimationBlender( );

private:

    Mesh* mesh;

    StateMachine state_machine;
    AnimationBlender< matrix4f > animation_blender;
};

#endif // _ENTITY_H_