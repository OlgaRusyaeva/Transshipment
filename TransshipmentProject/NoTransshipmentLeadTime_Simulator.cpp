//
//  NoTransshipmentLeadTime_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 23/09/15.
//
//

#include "NoTransshipmentLeadTime_Simulator.h"
#include "State.h"

float NoTransshipmentLeadTime_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration,  Actions& maxTransshipmentMatrix)
{
    return 0;
}

State NoTransshipmentLeadTime_Simulator::getInitialState(iProblem& problem,const int& iteration)
{
    return *initialState;
}
