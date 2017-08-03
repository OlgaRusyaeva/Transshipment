//
//  Static_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef __TransshipmentProject__Static_Simulator__
#define __TransshipmentProject__Static_Simulator__


#include "types.h"
#include "SimulatorOneTransshipment.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PiecewiseLinearFunctionSet;
class iProblem;

class Static_Simulator: public SimulatorOneTransshipment
{
public:
    Static_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const int& transshipPeriod):SimulatorOneTransshipment(numberOfIterations,seedNumber,NoTransshipmentFunction){tp=transshipPeriod;}
    ~Static_Simulator(){}
    
    bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration){if (tp==currentPeriod) return true; else return false;}
    
private:
    int tp;
    
};

#endif /* defined(__TransshipmentProject__Static_Simulator__) */
