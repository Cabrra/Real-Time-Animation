#ifndef _INTERPOLATOR_H_
#define _INTERPOLATOR_H_

#include <vector>
#include <math.h>
#include <algorithm>

#include "Animation.h"
#include "matrix4.h"
using namespace std;

#undef min
#undef max

template < typename T >
class Interpolator
{
public:

    Interpolator( );
    ~Interpolator( );

    void SetAnimation( const Animation< T >* animation );
    const Animation< T >* GetAnimation( ) const;

    void AddTime( float time );
    void SetTime( float time );
    float GetTime( ) const;

    virtual void Process( );

    const std::vector< T >& GetPose( ) const;

protected:

    void ProcessChannels( const std::vector< typename Animation< T >::Channel >& channels );
    void ProcessKeyframes( const std::vector< typename Animation< T >::KeyFrame >& key_frames );

    const Animation< T >* animation;
    float current_time;

    std::vector< T > pose;
};

template < typename T >
Interpolator< T >::Interpolator( )
{
    animation = 0;
    current_time = 0.0f;
}

template < typename T >
Interpolator< T >::~Interpolator( )
{
}

template < typename T >
void Interpolator< T >::SetAnimation( const Animation< T >* animation )
{
    // TODO
	//if (this->animation)
		//delete this->animation;
	this->animation = animation;
}

template < typename T >
const Animation< T >* Interpolator< T >::GetAnimation( ) const
{
    return animation;
}

template < typename T >
void Interpolator< T >::AddTime( float time )
{
    SetTime( current_time + time );
}

template < typename T >
void Interpolator< T >::SetTime( float time )
{
    // TODO
	current_time = time;
	if (current_time > animation->GetDuration())
		current_time -= animation->GetDuration();
	else if (current_time < 0)
		current_time += animation->GetDuration();


	if (current_time > animation->GetDuration())
		current_time = 0; 

}

template < typename T >
float Interpolator< T >::GetTime( ) const
{
    return current_time;
}

template < typename T >
void Interpolator< T >::Process( )
{
    if ( animation == 0 )
    {
        pose.clear( );
        return;
    }

    if ( animation->GetChannels( ).size( ) > 0 )
    {
        ProcessChannels( animation->GetChannels( ));
    }
    else
    {
        ProcessKeyframes( animation->GetKeyFrames( ));
    }
}

template < typename T >
const std::vector<T>& Interpolator< T >::GetPose( ) const
{
    return pose;
}

template < typename T >
void Interpolator< T >::ProcessChannels( const std::vector< typename Animation< T >::Channel >& channels )
{
    // TODO: Fill out the "pose" vector.  You are responsible for sizing it appropriately.
    //  "channels" contains an array of animation information for each joint.
    //  "current_time" contains our current time.

	size_t channelSIze = channels.size();
	pose.clear();
	for (size_t j = 0; j < channelSIze; j++)
	{
		size_t mySize = channels[j].keys.size();
		size_t next, previous;

		for (size_t i = 0; i < mySize; i++)
		{
			if (current_time < channels[j].keys[i].time)
			{
				next = i;
				if (i == 0)
				{
					previous = mySize - 1;
				}
				else
				{
					previous = i - 1;
				}
				break;
			}
		}

		float tween, alpha, framT;

		if (previous == 0)
		{
			framT = current_time - channels[j].keys[previous].time;
			tween = channels[j].keys[next].time - channels[j].keys[previous].time;
			alpha = framT / tween;
		}

		if (previous == 1)
		{
			framT = current_time - channels[j].keys[previous].time;
			tween = channels[j].keys[next].time - channels[j].keys[previous].time;
			alpha = framT / tween;
		}

		if (channels[j].keys[0].time > current_time)
		{
			framT = current_time;
			tween = channels[j].keys[next].time;
			alpha = framT / tween;
		}
		else
		{
			framT = current_time - channels[j].keys[previous].time;
			tween = channels[j].keys[next].time - channels[j].keys[previous].time;
			alpha = framT / tween;
		}

		T temp;
		temp = interpolate(channels[j].keys[previous].node, channels[j].keys[next].node, alpha);
		pose.push_back(temp);
		
	}

}

template < typename T >
void Interpolator< T >::ProcessKeyframes( const std::vector< typename Animation< T >::KeyFrame >& key_frames )
{
    // TODO: Fill out the "pose" vector.  You are responsible for sizing it appropriately.
    //  "key_frames" contains an array of poses.
    //  "current_time" contains our current time.
	
	size_t mySize = key_frames.size();
	size_t next, previous;
	
	for (size_t i = 0; i < mySize; i++)
	{
		if (current_time < key_frames[i].time)
		{
			next = i;
			if (i == 0)
			{
				previous = mySize - 1;
			}
			else
			{
				previous = i - 1;
			}
			break;
		}
	}

	float tween, alpha, framT;

	if (previous == 0)
	{
		framT = current_time - key_frames[previous].time;
		tween = key_frames[next].time - key_frames[previous].time;
		alpha = framT / tween;
	}

	if (previous == 1)
	{
		framT = current_time - key_frames[previous].time;
		tween = key_frames[next].time - key_frames[previous].time;
		alpha = framT / tween;
	}

	if (key_frames[0].time > current_time)
	{
		framT = current_time;
		tween = key_frames[next].time;
		alpha = framT / tween;
	}
	else
	{
		framT = current_time - key_frames[previous].time;
		tween = key_frames[next].time - key_frames[previous].time;
		alpha = framT / tween;
	}


	mySize = key_frames[0].nodes.size();
	pose.clear();
	for (size_t i = 0; i < mySize; i++)
	{
		T temp;
		temp = interpolate(key_frames[previous].nodes[i], key_frames[next].nodes[i], alpha);
		pose.push_back(temp);
	}

	//pose = key_frames[previous].nodes;
	
}

#endif // _INTERPOLATOR_H_