//
//  PiecewiseLinearFunction.h
//  TransshipmentProject
//
//  Created by Olga on 25/08/15.
//
//

#ifndef __TransshipmentProject__PiecewiseLinearFunction__
#define __TransshipmentProject__PiecewiseLinearFunction__

#include "types.h"

class PiecewiseLinearFunction {
    
public:
    PiecewiseLinearFunction();
    ~PiecewiseLinearFunction(){}
    
    void addSlope(const float& newSlope);
    void addBreakpoint(const int& newBreakpoint);
    void addIndex(const int& increment);
    
    void setSlope(const int& index, const float& newSlope);
    void setBreakpoint(const int& index,const int& newBreakpoint);
    void setIndex(const int& newIndex);
    
    void insertSlope(const int& position, const float& newSlope);
    void insertBreakpoint(const int& position, const int& newBreakpoint);
    
    float getSlope(const int& index) const;
    int getBreakpoint(const int& index) const;
    int getIndex() const;
    int getBreakpointClosestTo(const int& breakpoint) const;
    vector_t getBreakpoints() const;
    row_t getSlopes()const;
    
    int getOptimalBreakpoint(const float& decreaseInSlope) const;
    
    const PiecewiseLinearFunction& operator=(const PiecewiseLinearFunction& newPLF);

    
    
private:
    int max_index;
    vector_t breakpoints;
    row_t slopes;
    
};


#endif /* defined(__TransshipmentProject__PiecewiseLinearFunction__) */
