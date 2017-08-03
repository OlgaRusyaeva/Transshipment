//
//  ADPLeadTimeTransshipmentM_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#include "ADPLeadTimeTransshipmentM_Simulator.h"
#include "LP_optimizer.h"
#include "PLValueFunction.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "LeadTimes.h"


void ADPLeadTimeTransshipmentM_Simulator::initializePiecewiseAppVF(const boost::shared_ptr<PLValueFunction>& piecewiseVF)
{
    PVF=piecewiseVF;
}

void ADPLeadTimeTransshipmentM_Simulator::initializeInitialState(const boost::shared_ptr<State>& state)
{
    IL=state;
}

State ADPLeadTimeTransshipmentM_Simulator::getInitialState(iProblem& problem,const int& iteration)
{
    return *IL;
}

float ADPLeadTimeTransshipmentM_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix)
{
    Cost cost = problem.getCost();
    LeadTimes leadTimes =problem.getLeadTimes();
    
    int limit=leadTimes.getMaxLeadTimePlusOne();
    
    LP_optimizer lp_Problem;
    maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, PVF,currentPeriod,leadTimes,limit,false);
    
    problemMethods::getNewState(currentState);
    
    problemMethods::getNewState(currentState, maxTransshipmentMatrix,leadTimes);
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}