#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <vector>
#include <string>

template < typename T >
class Animation
{
public:

    struct Key
    {
        // The time this key happens at
        float time;

        // The node of this key
        T node;
    };

    struct Channel
    {
        std::vector< Key > keys;
    };

    struct KeyFrame
    {
        float time;
        std::vector< T > nodes;
    };

public:

    Animation( );
    ~Animation( );

    const std::string& GetName( ) const;
    float GetDuration( ) const;
    const std::vector< Channel >& GetChannels( ) const;
    const std::vector< KeyFrame >& GetKeyFrames( ) const;

public:

    std::string name;

    std::vector< Channel > channels;
    std::vector< KeyFrame > key_frames;
};

template < typename T >
Animation< T >::Animation( )
{

}

template < typename T >
Animation< T >::~Animation( )
{

}

template < typename T >
const std::string& Animation< T >::GetName( ) const
{
    return name;
}

template < typename T >
float Animation< T >::GetDuration( ) const
{
    if ( channels.size( ) > 0 )
        return channels.back( ).keys.back( ).time;
    else
        return key_frames.back( ).time;
}

template < typename T >
const std::vector< typename Animation< T >::Channel >& Animation< T >::GetChannels( ) const
{
    return channels;
}

template < typename T >
const std::vector< typename Animation< T >::KeyFrame >& Animation< T >::GetKeyFrames( ) const
{
    return key_frames;
}

#endif // _ANIMATION_H_