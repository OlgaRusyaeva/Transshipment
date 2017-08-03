//
//  BasisFunction.h
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#ifndef __TransshipmentProject__BasisFunction__
#define __TransshipmentProject__BasisFunction__

#include "types.h"
class State;

class BasisFunction
{
public:
    BasisFunction(const State& state);
    ~BasisFunction(){}
    
    row_t get()const;
    
    void set(const int& index, const int& value);
    
    int getSize()const;
    
    const BasisFunction& operator=(const BasisFunction& newBasicFunction);
    
private:
    row_t BF;
    
};

#endif /* defined(__TransshipmentProject__BasisFunction__) */
