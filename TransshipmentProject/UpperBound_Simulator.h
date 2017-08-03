//
//  UpperBound_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 10/03/15.
//
//

#ifndef TransshipmentProject_UpperBound_Simulator_h
#define TransshipmentProject_UpperBound_Simulator_h

#include "types.h"
#include "SimulatorOneTransshipment.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PiecewiseLinearFunctionSet;
class iProblem;

class UpperBound_Simulator: public SimulatorOneTransshipment
{
public:
    UpperBound_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const vector_t& transshipPeriod):SimulatorOneTransshipment(numberOfIterations,seedNumber,NoTransshipmentFunction){tp=vector_t(numberOfIterations);tp=transshipPeriod;}
    ~UpperBound_Simulator(){}
    
    bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration){if (tp[currentIteration]==currentPeriod) return true; else return false;}
    
private:
    vector_t tp;
    
};

#endif
