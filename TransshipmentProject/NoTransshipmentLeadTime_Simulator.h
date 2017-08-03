//
//  NoTransshipmentLeadTime_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 23/09/15.
//
//

#ifndef TransshipmentProject_NoTransshipmentLeadTime_Simulator_h
#define TransshipmentProject_NoTransshipmentLeadTime_Simulator_h

#include "types.h"
#include "SimulatorReplenishmentAndTransshipments.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class iProblem;


class NoTransshipmentLeadTime_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    NoTransshipmentLeadTime_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<State>& upperBound):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber){initialState=upperBound;}
    ~NoTransshipmentLeadTime_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration,  Actions& maxTransshipmentMatrix);
    
    State getInitialState(iProblem& problem,const int& iteration);
    
private:
        boost::shared_ptr<State> initialState;
};

#endif
