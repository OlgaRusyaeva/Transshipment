//
//  PerfectForesigtOneTransshipment_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 06/03/15.
//
//

#ifndef __TransshipmentProject__PerfectForesigtOneTransshipment_Simulator__
#define __TransshipmentProject__PerfectForesigtOneTransshipment_Simulator__


#include "types.h"
#include "SimulatorOneTransshipment.h"

class State;
class Cost;
class Actions;
class Demand;
class PiecewiseApp_ValueFunction;
class iProblem;

class PerfectForesigtOneTransshipment_Simulator: public SimulatorOneTransshipment
{
public:
    PerfectForesigtOneTransshipment_Simulator(const int& numberOfIterations, const int& seedNumber,const PiecewiseApp_ValueFunction& NoTransshipmentFunction, const vector_t& transshipPeriods):SimulatorOneTransshipment(numberOfIterations,seedNumber,NoTransshipmentFunction){initializeTransshipmentPeriodVector(transshipPeriods);}
    
    bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration);
private:
    vector_t tp;
    
     void initializeTransshipmentPeriodVector(const vector_t& transshipPeriods);
    
};

#endif /* defined(__TransshipmentProject__PerfectForesigtOneTransshipment_Simulator__) */
