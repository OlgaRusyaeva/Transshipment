//
//  Deterministic_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 7/31/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Deterministic_Simulator.h"
#include "ActionsMap.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "State.h"
#include "Actions.h"
#include "iDistribution.h"
#include "LeadTimes.h"

void Deterministic_Simulator::initializeActionsMap(const std::vector<actionMap_ptr>& a_map)
{
    for (int n=0; n<iterations; n++) {
        actionMap_ptr ptr(new ActionsMap(a_map[n]->getLocationParameter(), a_map[n]->getTimeParameter()));
        AM.push_back(a_map[n]);
    }
}

float Deterministic_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    /*for (int i=0; i<location; i++) {
        for (int j=0; j<location; j++) {
            maxTransshipmentMatrix.setElement(i,j,AM->getActionPerPeriodBetweenLocations(currentPeriod,i,j));
        }
    }*/
    LeadTimes leadTimes=problem.getLeadTimes();
    
    maxTransshipmentMatrix=AM[currentIteration]->getActionPerPeriod(currentPeriod);
    problemMethods::getNewState(currentState, maxTransshipmentMatrix,leadTimes);
    return problemMethods::getTransshipmentCost(problem.getCost(), maxTransshipmentMatrix);  
    
}
