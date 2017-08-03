//
//  PerfectForesigtOneTransshipment_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 06/03/15.
//
//

#include "PerfectForesigtOneTransshipment_Simulator.h"
#include "iProblem.h"
#include "State.h"
#include "PiecewiseApp_ValueFunction.h"


void PerfectForesigtOneTransshipment_Simulator::initializeTransshipmentPeriodVector(const vector_t& transshipmentPeriods)
{
    tp=vector_t((int)transshipmentPeriods.size());
    tp=transshipmentPeriods;

}

bool PerfectForesigtOneTransshipment_Simulator::timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)
{
    if (tp[currentIteration==currentPeriod]) {
        return true;
    }
    else
        return false;
}
