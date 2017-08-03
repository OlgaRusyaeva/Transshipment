//
//  PiecewiseLinearFunctionSet.h
//  TransshipmentProject
//
//  Created by Olga on 25/08/15.
//
//

#ifndef __TransshipmentProject__PiecewiseLinearFunctionSet__
#define __TransshipmentProject__PiecewiseLinearFunctionSet__

#include "PiecewiseLinearFunction.h"
#include "types.h"
#include "array.h"


class PiecewiseLinearFunctionSet
{
public:
    PiecewiseLinearFunctionSet(int sizeH, int sizeW):set(sizeW,sizeH){}
    ~PiecewiseLinearFunctionSet(){}
    
    PiecewiseLinearFunction& get(const int& indexH, const int& indexW);
    
    int getSizeH() const;
    
    int getSizeW() const;
    
    void printSlopes();
    
    void printBreakpoints();
    
    void printIndexes();

    
private:
	arrays::Array2D<PiecewiseLinearFunction> set;
    
    
};

#endif /* defined(__TransshipmentProject__PiecewiseLinearFunctionSet__) */
