#ifndef _ANIMATIONBLENDER_H_
#define _ANIMATIONBLENDER_H_

#include <algorithm>

#include "Animation.h"
#include "Interpolator.h"

template < typename T >
class AnimationBlender
{
public:

    AnimationBlender( );
    ~AnimationBlender( );

    void SnapTo( Animation< T >* animation );
    void TransitionTo( Animation< T >* animation, float transition_time );

    void Process( float delta_time );

    const std::vector< T >& GetPose( ) const;
    bool IsBlending( ) const;

private:

    Interpolator< T > interpolators[2];
    std::vector< T > pose;

    // Our current interpolator index
    unsigned int current_interpolator;

    // If transition time is greater than 0.0f, we are blending
    float transition_time;

    // Elapsed transition time
    float elapsed_transition_time;
};

template < typename T >
AnimationBlender< T >::AnimationBlender( )
{
    current_interpolator = 0;
    transition_time = 0.0f;
    elapsed_transition_time = 0.0f;
}

template < typename T >
AnimationBlender< T >::~AnimationBlender( )
{
}

template < typename T >
void AnimationBlender< T >::SnapTo( Animation< T >* animation )
{
    transition_time = 0.0f;
    elapsed_transition_time = 0.0f;
    interpolators[current_interpolator].SetAnimation( animation );
}

template < typename T >
void AnimationBlender< T >::TransitionTo( Animation< T >* animation, float transition_time )
{
    unsigned int next_interpolator;

    if ( interpolators[current_interpolator].GetAnimation( ) == 0 )
    {
        // No current animation
        SnapTo( animation );
    }
    else
    {
        next_interpolator = current_interpolator ^ 0x1;
        this->transition_time = transition_time;
        this->elapsed_transition_time = 0.0f;

        interpolators[next_interpolator].SetAnimation( animation );
    }
}

template < typename T >
void AnimationBlender< T >::Process( float delta_time )
{
    // TODO:  Fill out interpolated_joints.  You are responsible for sizing it appropriately.
    //
    //   Calculate the next interpolator index from the current interpolator index.  This AnimationBlender has two
    //     interpolators, and we cycle through them when blending.  For example, if the current interpolator index
    //     ( current_interpolator ) is 0, then the next interpolator index would be 1.  If the current interpolator
    //     index is 1, then the next interpolator index would be 0.
    //
    //   If the transition_time is greater than 0.0f, we are blending:
    //     Increment the elapsed_transition_time by the incoming delta_time
    //     If the elapsed_transition_time is greater than or equal to the transition time:
    //       Set the current interpolator index to the next interpolator index
    //       Zero out transition time and elapsed_transition_time
    //
    //   Call AddTime and Process on the current interpolator
    //
    //   If the transition time is still greater than 0.0f:
    //     Call AddTime and Process on the next interpolator
    //     Fill out interpolated_joints by interpolating between the joints of the current and next interpolators
    //   If the transition time is less than or equal to 0.0f:
    //     Fill out the interpolated_joints from the current interpolator's joints



	if (transition_time > 0)
	{
		elapsed_transition_time += delta_time;
		if (elapsed_transition_time >= transition_time)
		{
			current_interpolator ^= 1;
			elapsed_transition_time = transition_time = 0;
		}
	}

	interpolators[current_interpolator].AddTime(delta_time);
	interpolators[current_interpolator].Process();

	if (transition_time > 0)
	{
		interpolators[current_interpolator ^ 1].AddTime(delta_time);
		interpolators[current_interpolator ^ 1].Process();
		//interpolate joints between two interpolators

		float lamda;
		
		lamda = elapsed_transition_time / transition_time;

		vector<matrix4f> curPose = interpolators[current_interpolator].GetPose();
		vector<matrix4f> nextPose = interpolators[current_interpolator ^ 1].GetPose();
		size_t mySiz = curPose.size();
		pose.resize(mySiz);
		for (size_t i = 0; i < mySiz; i++)
		{
			matrix4f myInter;
			myInter = interpolate(curPose[i], nextPose[i], lamda);
			pose[i] = myInter;
		}
	}
	else
	{
		//interpolate joints from current interpolator
		vector<matrix4f> curPose = interpolators[current_interpolator].GetPose();
		size_t mySiz = curPose.size();
		pose.resize(mySiz);
		for (size_t i = 0; i < mySiz; i++)
			pose[i] = curPose[i];
	}
}

template < typename T >
const std::vector< T >& AnimationBlender< T >::GetPose( ) const
{
    return pose;
}

template < typename T >
bool AnimationBlender< T >::IsBlending( ) const
{
    return transition_time > 0.0f;
}

#endif // _ANIMATIONBLENDER_H_