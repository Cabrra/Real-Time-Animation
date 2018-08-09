#ifndef _PROC_INTERPOLATOR_H_
#define _PROC_INTERPOLATOR_H_

#include "Interpolator.h"
#include "matrix4.h"

class ProcInterpolator : public Interpolator< matrix4f >
{
    float angle_total;
    float angle;

public:

    ProcInterpolator( );
    ~ProcInterpolator( );

    virtual void Process( );
};

#endif // _PROC_INTERPOLATOR_H_