#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

#include <vector>

template < typename T >
class Animation;

template < typename T >
class AnimationManager
{
private:

    AnimationManager( );

    // Disable copy constructor and assignment operator
    AnimationManager( const AnimationManager& a );
    const AnimationManager& operator=( const AnimationManager& a );

public:

    ~AnimationManager( );

    static AnimationManager< T >* GetInstance( );

    void Add( Animation< T >* animation );
    void Remove( Animation< T >* animation );
    void Remove( std::string name );
    void RemoveAll( );

    Animation< T >* GetAnimation( std::string name );
    Animation< T >* GetAnimation( size_t index );

private:

    // Shallow pointers
    std::vector< Animation< T >* > animations;
};

template < typename T >
AnimationManager< T >::AnimationManager( )
{
}

template < typename T >
AnimationManager< T >::~AnimationManager( )
{
}

template < typename T >
AnimationManager< T >* AnimationManager< T >::GetInstance( )
{
    static AnimationManager< T > instance;
    return &instance;
}

template < typename T >
void AnimationManager< T >::Add( Animation< T >* animation )
{
    if ( animation != 0 )
    {
        animations.push_back( animation );
    }
}

template < typename T >
void AnimationManager< T >::Remove( Animation< T >* animation )
{
    size_t i, size;

    size = animations.size( );
    for ( i = 0; i < size; ++i )
    {
        if ( animation == animations[i] )
        {
            animations.erase( animations.begin( ) + i );
            break;
        }
    }
}

template < typename T >
void AnimationManager< T >::Remove( std::string name )
{
    size_t i, size;

    size = animations.size( );
    for ( i = 0; i < size; ++i )
    {
        if ( name == animations[i]->GetName( ))
        {
            animations.erase( animations.begin( ) + i );
            break;
        }
    }
}

template < typename T >
void AnimationManager< T >::RemoveAll( )
{
    animations.clear( );
}

template < typename T >
Animation< T >* AnimationManager< T >::GetAnimation( std::string name )
{
    size_t i, size;

    size = animations.size( );
    for ( i = 0; i < size; ++i )
    {
        if ( name == animations[i]->GetName( ))
        {
            return animations[i];
        }
    }

    return 0;
}

template < typename T >
Animation< T >* AnimationManager< T >::GetAnimation( size_t index )
{
    if ( index >= animations.size( ))
    {
        return 0;
    }
    else
    {
        return animations[index];
    }
}

#endif // _ANIMATION_MANAGER_H_