//
//  PerfectForesightOneTransshipment_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 06/03/15.
//
//

#ifndef __TransshipmentProject__PerfectForesightOneTransshipment_Solver__
#define __TransshipmentProject__PerfectForesightOneTransshipment_Solver__

#include "types.h"

class PiecewiseApp_ValueFunction;
class iProblem;
class State;
class Cost;
class Demand;
class Actions;


class PerfectForesightOneTransshipment_Solver
{
public:
    PerfectForesightOneTransshipment_Solver(const int& numberOfIterations, const int& seedNumber,const PiecewiseApp_ValueFunction& NoTransshipmentFunction){seed=seedNumber;iterations=numberOfIterations;initializePiecewiseAppVF(NoTransshipmentFunction);initializeTransshipmentPeriodVector(numberOfIterations);}
    ~PerfectForesightOneTransshipment_Solver();
    
    vector_t& getTransshipmentPeriods();
    
    void solve(iProblem& problem);
    
private:
    int seed;
    int iterations;
    PiecewiseApp_ValueFunction *F;
    vector_t transshipmentPeriods;
    
    void initializePiecewiseAppVF(const PiecewiseApp_ValueFunction& piecewiseVF);
    
    void initializeTransshipmentPeriodVector(const int& numberOfIterations);
    
    void getVectorOfTransshipmentPeriods(const int& location, const int& time, const State& initialState,const Cost& cost,Demand& demand);

    float getTransshipmentCost(const Cost& cost,const int& currentPeriod, const State& currentState, Actions& maxTransshipmentMatrix)const;
    
    
};

#endif /* defined(__TransshipmentProject__PerfectForesightOneTransshipment_Solver__) */
