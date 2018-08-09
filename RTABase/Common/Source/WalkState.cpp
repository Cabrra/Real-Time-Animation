#include "WalkState.h"

#include <windows.h>

#include "AnimationManager.h"
#include "Entity.h"
#include "IdleState.h"

WalkState::WalkState( )
{
}

WalkState::~WalkState( )
{
}

WalkState* WalkState::GetInstance( )
{
    static WalkState state;
    return &state;
}

void WalkState::Enter( Entity* entity )
{
    // TODO:  Using the entity's animation blender, transition to the walk animation over a time of 1.0f seconds
    //   You can get the Walk animation using the singleton AnimationManager class.
	Animation<matrix4f>* myAnim = AnimationManager<matrix4f>::GetInstance()->GetAnimation("Walk");
	AnimationBlender<matrix4f>& myBlend = entity->GetAnimationBlender();
	myBlend.TransitionTo(myAnim, 1.0f);
}

void WalkState::Execute( Entity* entity, float delta_time )
{
    // TODO:  Transition to the IdleState if the 'W' key is NOT pressed and we are not currently blending
	AnimationBlender<matrix4f>& myBlend = entity->GetAnimationBlender();
	if (!(GetKeyState('W') & 0x8000) && !myBlend.IsBlending())
	{
		Animation<matrix4f>* myAnim = AnimationManager<matrix4f>::GetInstance()->GetAnimation("Idle");
		//myBlend.SnapTo(myAnim);
		entity->GetStateMachine().ChangeState(IdleState::GetInstance());
	}
}

void WalkState::Exit( Entity* entity )
{
}