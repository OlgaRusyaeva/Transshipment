//
//  SimulatorReplenishmentAndTransshipments.h
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#ifndef __TransshipmentProject__SimulatorReplenishmentAndTransshipments__
#define __TransshipmentProject__SimulatorReplenishmentAndTransshipments__

#include"types.h"

class iProblem;
class Profit;
class State;
class Actions;
class Demand;
class Cost;
class iDistribution;

class SimulatorReplenishmentAndTransshipments
{
public:
    
    SimulatorReplenishmentAndTransshipments(const int& numberOfIterations, const int& seedNumber){seed=seedNumber;iterations=numberOfIterations;}
    
    virtual ~SimulatorReplenishmentAndTransshipments(){}
    
    void simulate(iProblem& problem, Profit& profit);
    
    virtual float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix)=0;
    
    virtual State getInitialState(iProblem& problem,const int& iteration)=0;
    
    
protected:
    int seed;
    int iterations;
    
};

#endif /* defined(__TransshipmentProject__SimulatorReplenishmentAndTransshipments__) */
