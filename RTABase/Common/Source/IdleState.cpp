#include "IdleState.h"

#include <windows.h>

#include "AnimationManager.h"
#include "Entity.h"
#include "WalkState.h"

IdleState::IdleState( )
{
}

IdleState::~IdleState( )
{
}

IdleState* IdleState::GetInstance( )
{
    static IdleState instance;
    return &instance;
}

void IdleState::Enter( Entity* entity )
{
    // TODO:  Using the entity's animation blender, transition to the idle animation over a time of 1.0f seconds
    //   You can get the Idle animation using the singleton AnimationManager class.
	Animation<matrix4f>* myAnim = AnimationManager<matrix4f>::GetInstance()->GetAnimation("Idle");
	AnimationBlender<matrix4f>& myBlend = entity->GetAnimationBlender();
	myBlend.TransitionTo(myAnim, 1.0f);
}

void IdleState::Execute( Entity* entity, float delta_time )
{
    // TODO:  Transition to the WalkState if the 'W' key is pressed and we are not currently blending
	AnimationBlender<matrix4f>& myBlend = entity->GetAnimationBlender();
	if ((GetKeyState('W') & 0x8000) && !myBlend.IsBlending())
	{
		Animation<matrix4f>* myAnim = AnimationManager<matrix4f>::GetInstance()->GetAnimation("Walk");
		//myBlend.SnapTo(myAnim);
		entity->GetStateMachine().ChangeState(WalkState::GetInstance());
	}

}

void IdleState::Exit( Entity* entity )
{
}