//
//  ADPLifeTimeTransshipment_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 22/06/16.
//
//

#include "ADPLifeTimeTransshipment_Simulator.hpp"
#include "LP_optimizer.h"
#include "PLValueFunction.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ProblemMethods.h"
#include "iProblem.h"


void ADPLifeTimeTransshipment_Simulator::initializePiecewiseAppVF(const boost::shared_ptr<PLValueFunction>& piecewiseVF)
{
    PVF=piecewiseVF;
}

float ADPLifeTimeTransshipment_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    Cost cost = problem.getCost();
    
    LP_optimizer lp_Problem;
    maxTransshipmentMatrix = lp_Problem.optimize(currentState,0, cost, (*PVF).getPiecewiseLinearFunctionFor(0),currentPeriod);
    
    problemMethods::getNewState(currentState, maxTransshipmentMatrix);
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}