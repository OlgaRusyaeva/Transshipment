//
//  BFValueFunction.h
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#ifndef __TransshipmentProject__BFValueFunction__
#define __TransshipmentProject__BFValueFunction__

#include "types.h"

class BFValueFunction
{
public:
    BFValueFunction(){}
    ~BFValueFunction(){}
    
    void initialize(const int& time);
    
    void set(const int& timeIndex, const row_t& coefficients);
    
    matrix_t get()const;
    
    const BFValueFunction& operator=(const BFValueFunction& newValueFunction);
    
private:
    matrix_t TC;
    
};

#endif /* defined(__TransshipmentProject__BFValueFunction__) */
