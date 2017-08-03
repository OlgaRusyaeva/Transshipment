//
//  NumberOperations.h
// 

#include "NumbersOperations.h"
#include <math.h> 

namespace numbersOperations {
    
    float getMinValue(const float& lft, const float& rht)
    {
        if (lft<rht) return lft;
        else return rht;
    }
    
    // getMaxValue = (-1)*getMinValue(-lft,-rht)
    float getMaxValue(const float& lft, const float& rht)
    {
        if (lft>rht) return lft;
        else return rht;
        
    }
    
    bool definitelyLessThan(float a, float b, float epsilon)
    {
        return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }
    
    bool approximatelyEqual(float a, float b, float epsilon)
    {
        return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }
    
    bool definitelyGreaterThan(float a, float b, float epsilon)
    {
        return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }
    
}


