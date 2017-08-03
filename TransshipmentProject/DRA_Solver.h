//
//  DRA_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 07/03/15.
//
//

#ifndef __TransshipmentProject__DRA_Solver__
#define __TransshipmentProject__DRA_Solver__

#include "types.h"

class PiecewiseApp_ValueFunction;
class State;
class Actions;
class Cost;
class iProblem;
class Demand;
class iDistribution;

class DRA_Solver
{
public:
    DRA_Solver(const PiecewiseApp_ValueFunction& NoTransshipmentFunction, const int& iterations, const int& seedNumber){initializePiecewiseAppVF(NoTransshipmentFunction);iterationNumber=iterations;initializeTransshipPeriods(iterations);seed=seedNumber;}
    ~DRA_Solver();
    
    void solve(iProblem& problem);
    
    vector_t getTransshipPeriods()const;
    
private:
    PiecewiseApp_ValueFunction *F;
    vector_t transshipPeriods;
    
    int iterationNumber;
    int seed;
    int replicationNumber;
    vector_t replicationVector;
    
    void initializePiecewiseAppVF(const PiecewiseApp_ValueFunction& piecewiseVF);
    void initializeTransshipPeriods(const int& iterations);
    
    float getExpectedProfitWithCurrentTransshipment(const Cost& cost,const int& currentPeriod, const State& currentState)const;
    
    float getExpectedProfit(iProblem& problem,const int& currentPeriod, const State& currentState, Demand& demand, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
};

#endif /* defined(__TransshipmentProject__DRA_Solver__) */
