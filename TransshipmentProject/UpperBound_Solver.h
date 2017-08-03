//
//  UpperBound_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 10/03/15.
//
//

#ifndef __TransshipmentProject__UpperBound_Solver__
#define __TransshipmentProject__UpperBound_Solver__

#include "types.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class PiecewiseLinearFunctionSet;
class Actions;
class iProblem;

class UpperBound_Solver
{
public:
    UpperBound_Solver(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const int& iterationNumber, const int& seedNumber){initializePiecewiseAppVF(NoTransshipmentFunction);seed=seedNumber;iterations=iterationNumber;initializeTransshipPeriods(iterationNumber);}
    ~UpperBound_Solver();
    
    vector_t getTransshipPeriod()const;
    
    void solve(iProblem& problem);
    
private:
     boost::shared_ptr<PiecewiseLinearFunctionSet> F;
    vector_t transshipPeriods;
    int iterations;
    int seed;
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& piecewiseVF);
    void initializeTransshipPeriods(const int& iterationNumber);
    
    
};


#endif /* defined(__TransshipmentProject__UpperBound_Solver__) */
