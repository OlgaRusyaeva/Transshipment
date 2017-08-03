//
//  BasisFunctionApproximation_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#ifndef __TransshipmentProject__BasisFunctionApproximation_Simulator__
#define __TransshipmentProject__BasisFunctionApproximation_Simulator__

#include "types.h"
#include "SimulatorOneTransshipment.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PiecewiseLinearFunctionSet;
class iProblem;
class BFValueFunction;

class BasisFunctionApproximation_Simulator: public SimulatorOneTransshipment
{
public:
    BasisFunctionApproximation_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const BFValueFunction& valueFunction):SimulatorOneTransshipment(numberOfIterations,seedNumber,NoTransshipmentFunction){initializeValueFunction(valueFunction);}
    ~BasisFunctionApproximation_Simulator();
    
    bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration);
    
private:
    BFValueFunction *VF;
    
    void initializeValueFunction(const BFValueFunction& valueFunction);
    
    float getExpectedProfitNextPeriodsWithTransshipments(const int& currentPeriod, const State& state);
    
};


#endif /* defined(__TransshipmentProject__BasisFunctionApproximation_Simulator__) */
