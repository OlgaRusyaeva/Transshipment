//
//  DPOneTransshipmentPeriod_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef __TransshipmentProject__DPOneTransshipmentPeriod_Simulator__
#define __TransshipmentProject__DPOneTransshipmentPeriod_Simulator__

#include "types.h"
#include "SimulatorOneTransshipment.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PiecewiseLinearFunctionSet;
class iProblem;
class Hashing;
class ValueFunction;

class DPOneTransshipment_Simulator: public SimulatorOneTransshipment
{
public:
    DPOneTransshipment_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const ValueFunction& valueFunction,const Hashing& hashFunction):SimulatorOneTransshipment(numberOfIterations,seedNumber,NoTransshipmentFunction){initializeValueFunction(valueFunction);initializeHashFunction(hashFunction);}
    ~DPOneTransshipment_Simulator();
    
    bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration);
    
private:
    ValueFunction *VF;
    Hashing *HF;
    
    void initializeValueFunction(const ValueFunction& valueFunction);
    
    void initializeHashFunction(const Hashing& hashFunction);
    
    float getExpectedProfitNextPeriodsWithTransshipments(const int& location,const Cost& cost, Demand& demand,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
};

#endif /* defined(__TransshipmentProject__DPOneTransshipmentPeriod_Simulator__) */
