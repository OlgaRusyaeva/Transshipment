//
//  DRA_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#include "DRA_Simulator.h"
#include "iProblem.h"
#include "State.h"
#include "PiecewiseLinearFunctionSet.h"
#include "Actions.h"
#include "Cost.h"
#include "Demand.h"
#include "iDistribution.h"
#include "ProblemMethods.h"
#include "NumbersOperations.h"

/*bool DRA_Simulator::timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)
{
    if (currentPeriod==tp[currentIteration]) {
        return true;
    }
    else
        return false;
}*/


bool DRA_Simulator::timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)
{
    bool flag=false;
    bool isZero=false;
    
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    State state=currentState;
    Demand demand=problem.getDemand();
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed+currentIteration+currentPeriod+20);
    
    float expectedProfitNextPeriodsWithCurrentTransshipment=getExpectedProfitWithCurrentTransshipment(cost, currentPeriod, state,isZero);

    float expectedProfitNextPeriodsWithTransshipments=getExpectedProfitNextPeriodsWithTransshipments(problem, currentPeriod, currentState, currentIteration, distribution);
    
    
    if ((!isZero)&&(expectedProfitNextPeriodsWithCurrentTransshipment>=expectedProfitNextPeriodsWithTransshipments))
    {
        flag=true;
    }
    
    
    return flag;
}


float DRA_Simulator::getExpectedProfitNextPeriodsWithTransshipments(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    
    //do it based on the approximation
    if (currentPeriod==problem.getTimeHorizon()-1) {
        float expectedProfitLastPeriod=getExpectedProfitNextPeriodsWithoutTransshipments(location,currentPeriod,currentState);
        //for (int i=0; i<location; i++) {
        //expectedProfitLastPeriod+=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-expectedValuePositivePart(1,distribution[i],currentState.getElement(i)))-cost.getHoldingCost()[i]*currentState.getElement(i);
        //}
        return expectedProfitLastPeriod;
    }
    else
    {
        Demand demand=problem.getDemand();
        
        float currentProfit=0;
        float expectedProfit=0;
        bool isZero=false;
        
        for (int r=0; r<rn; r++) {
            
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
            }
            
            float reward = problemMethods::getReward(cost.getSalesCost(),currentState,demand);
            
            State stateAfterDemandSatisfaction=currentState;
            
            problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
            
            float holdingCost = problemMethods::getCost(cost.getHoldingCost(),stateAfterDemandSatisfaction);
            
            currentProfit+=reward-holdingCost;
            
            State stateAfterActions=stateAfterDemandSatisfaction;
            
            float expectedProfitNextPeriodsWithCurrentTransshipment=getExpectedProfitWithCurrentTransshipment(cost, currentPeriod+1, stateAfterActions,isZero);
            
            expectedProfit+=numbersOperations::getMaxValue(expectedProfitNextPeriodsWithCurrentTransshipment,getExpectedProfitNextPeriodsWithTransshipments(problem,currentPeriod+1,stateAfterDemandSatisfaction,currentIteration,distribution));
        }
        
        return (currentProfit+expectedProfit)/rn;
    }
    
    
    
}
