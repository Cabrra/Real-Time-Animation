#include "Entity.h"

Entity::Entity( )
{
    mesh = 0;

    state_machine.SetEntity( this );
}

Entity::~Entity( )
{
}

void Entity::Update( float delta_time )
{
    state_machine.Update( delta_time );
    animation_blender.Process( delta_time );
}

void Entity::SetMesh( Mesh* mesh )
{
    this->mesh = mesh;
}

Mesh* Entity::GetMesh( )
{
    return mesh;
}

StateMachine& Entity::GetStateMachine( )
{
    return state_machine;
}

AnimationBlender< matrix4f >& Entity::GetAnimationBlender( )
{
    return animation_blender;
}
