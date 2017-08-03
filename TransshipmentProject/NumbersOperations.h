//
//  Header NumberOperations.cpp
//  


#ifndef project3_NumberOperations_h
#define project3_NumberOperations_h

namespace numbersOperations {
    //get min/max value between two integers
    float getMaxValue(const float& lft, const float& rht);
    float getMinValue(const float& lft, const float& rht);
    
    bool definitelyLessThan(float a, float b, float epsilon);
    bool approximatelyEqual(float a, float b, float epsilon);
    bool definitelyGreaterThan(float a, float b, float epsilon);
};


#endif
