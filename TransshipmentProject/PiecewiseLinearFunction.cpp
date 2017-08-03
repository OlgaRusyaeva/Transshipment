//
//  PiecewiseLinearFunction.cpp
//  TransshipmentProject
//
//  Created by Olga on 25/08/15.
//
//

#include "PiecewiseLinearFunction.h"

PiecewiseLinearFunction::PiecewiseLinearFunction()
{
    max_index=1;
    breakpoints.push_back(0);
    slopes.push_back(0);
}

void PiecewiseLinearFunction::addSlope(const float& newSlope)
{
    slopes.push_back(newSlope);
}

void PiecewiseLinearFunction::addBreakpoint(const int& newBreakpoint)
{
    breakpoints.push_back(newBreakpoint);
}

void PiecewiseLinearFunction::addIndex(const int& increment)
{
    max_index+=increment;
}

void PiecewiseLinearFunction::setSlope(const int& index, const float& newSlope)
{
    slopes[index]=newSlope;
}

void PiecewiseLinearFunction::setBreakpoint(const int& index,const int& newBreakpoint)
{
    breakpoints[index]=newBreakpoint;
}

void PiecewiseLinearFunction::setIndex(const int& newIndex)
{
    max_index=newIndex;
}

void PiecewiseLinearFunction::insertSlope(const int& position, const float& newSlope)
{
    slopes.insert(slopes.begin()+position,newSlope);
}

void PiecewiseLinearFunction::insertBreakpoint(const int& position, const int& newBreakpoint)
{
    breakpoints.insert(breakpoints.begin()+position,newBreakpoint);
}

float PiecewiseLinearFunction::getSlope(const int& index) const
{
    return slopes[index];
}

int PiecewiseLinearFunction::getBreakpoint(const int& index) const
{
    return breakpoints[index];
}

int PiecewiseLinearFunction::getIndex() const
{
    return max_index;
}

int PiecewiseLinearFunction::getBreakpointClosestTo(const int& breakpoint) const
{
    if (breakpoint==0) {
        return 0;
    }
    else
    {
        if (breakpoint==breakpoints[breakpoint]) {
            return breakpoint;
        }
        else
        {
            return std::lower_bound(breakpoints.begin(),breakpoints.end(),breakpoint)-breakpoints.begin()-1;
        }
    }
}


const PiecewiseLinearFunction& PiecewiseLinearFunction::operator=(const PiecewiseLinearFunction& newPLF)
{
    max_index=newPLF.getIndex();
    breakpoints=newPLF.getBreakpoints();
    slopes=newPLF.getSlopes();

    
    return *this;
}

vector_t PiecewiseLinearFunction::getBreakpoints() const
{
    return breakpoints;
}

row_t PiecewiseLinearFunction::getSlopes()const
{
    return slopes;
}

int PiecewiseLinearFunction::getOptimalBreakpoint(const float& decreaseInSlope) const
{
    int optimalBreakpoint=0;
    for (int i=0; i<max_index; i++) {
        if ((i==max_index-1)||(slopes[i]-decreaseInSlope*(breakpoints[i+1]-breakpoints[i])<0)) {
            optimalBreakpoint=breakpoints[i];
            break;
        }
    }
    return optimalBreakpoint;
}