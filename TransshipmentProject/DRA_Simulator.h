//
//  DRA_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef __TransshipmentProject__DRA_Simulator__
#define __TransshipmentProject__DRA_Simulator__

#include "types.h"
#include "SimulatorOneTransshipment.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PiecewiseLinearFunctionSet;
class iProblem;

class DRA_Simulator: public SimulatorOneTransshipment
{
public:
    DRA_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const int& replicationNumber):SimulatorOneTransshipment(numberOfIterations,seedNumber,NoTransshipmentFunction){rn=replicationNumber;}
    
    bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration);
    
private:
    int rn;
    
    float getExpectedProfitNextPeriodsWithTransshipments(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
};

#endif /* defined(__TransshipmentProject__DRA_Simulator__) */
