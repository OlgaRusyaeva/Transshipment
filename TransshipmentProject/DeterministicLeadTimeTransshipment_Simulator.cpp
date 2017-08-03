//
//  DeterministicLeadTimeTransshipment_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#include "DeterministicLeadTimeTransshipment_Simulator.h"
#include "ActionsMap.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "State.h"
#include "Actions.h"
#include "iDistribution.h"
#include "LeadTimes.h"

void DeterministicLeadTimeTransshipment_Simulator::initializeTransshipments(const std::vector<actionMap_ptr>& transshipments)
{
    for (int n=0; n<iterations; n++) {
        actionMap_ptr ptr(new ActionsMap(transshipments[n]->getLocationParameter(), transshipments[n]->getTimeParameter()));
        TR.push_back(transshipments[n]);
    }
}

void DeterministicLeadTimeTransshipment_Simulator::initializeOrders(const std::vector<order_ptr>& orders)
{
    for (int n=0; n<iterations; n++) {
        order_ptr ptr(new State(orders[n]->getLength()));
        IL.push_back(orders[n]);
    }
}

float DeterministicLeadTimeTransshipment_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration,  Actions& maxTransshipmentMatrix)
{
    LeadTimes leadTimes=problem.getLeadTimes();
    
    maxTransshipmentMatrix=TR[currentIteration]->getActionPerPeriod(currentPeriod);
    problemMethods::getNewState(currentState);
    problemMethods::getNewState(currentState, maxTransshipmentMatrix,leadTimes);
    return problemMethods::getTransshipmentCost(problem.getCost(), maxTransshipmentMatrix);
    
}

State DeterministicLeadTimeTransshipment_Simulator::getInitialState(iProblem& problem,const int& iteration) 
{
    return *IL[iteration];
}