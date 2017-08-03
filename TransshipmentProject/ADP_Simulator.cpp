//
//  ADP_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ADP_Simulator.h"
#include "LP_optimizer.h"
#include "PLValueFunction.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ProblemMethods.h"
#include "iProblem.h"


void ADP_Simulator::initializePiecewiseAppVF(const boost::shared_ptr<PLValueFunction>& piecewiseVF)
{
    PVF=piecewiseVF;
}
      
 float ADP_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    Cost cost = problem.getCost();
    int location = problem.getStateDimension();
    int lifeTime= problem.getLifeTime();
    
    Actions lifeTimeActions(location);
    for (int m=0; m<lifeTime; m++) {
        //solve LP problem for each m
        LP_optimizer lp_Problem;
        maxTransshipmentMatrix = lp_Problem.optimize(currentState, m,cost, (*PVF).getPiecewiseLinearFunctionFor(m), currentPeriod);
    
        problemMethods::getNewState(currentState, maxTransshipmentMatrix,m);
        lifeTimeActions+=maxTransshipmentMatrix;
    }
    
    //sum actions
    float transshipmentCost = problemMethods::getTransshipmentCost(cost,lifeTimeActions);
        
    return transshipmentCost;
        
}
 
    

