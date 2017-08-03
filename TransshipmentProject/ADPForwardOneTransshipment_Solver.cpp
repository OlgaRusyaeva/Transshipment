//
//  ADPForwardOneTransshipment_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 04/03/15.
//
//

#include "ADPForwardOneTransshipment_Solver.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "PiecewiseLinearFunctionSet.h"
#include "LP_optimizer.h"
#include "Actions.h"
#include "array.h"
#include "ProblemMethods.h"
#include "ValueFunction.h"
#include "Hashing.h"

using namespace std;
using namespace arrays;


ADPForwardOneTransshipment_Solver::~ADPForwardOneTransshipment_Solver()
{
    //delete F;
}


float ADPForwardOneTransshipment_Solver::getCostForActions(State& state,const int& currentPeriod,const Cost& cost, const int& time, const arrays::Array2D<float>& possibleDemands)const
{
    const int location=state.getLength();
    
    Actions transshipmentMatrix(location);
    
    float expectedProfitNextPeriodsWithTransshipments=getTransshipmentCost(cost, currentPeriod, state, transshipmentMatrix);
    
    float nextPeriodProfit=0;
    
    
    Demand demand(location);
    
   
    for (int d=0; d<possibleDemands.sizeH(); d++)
    {
		for (int l=0; l<location; l++) {
            demand.setElement(l, possibleDemands(d,l));
        }
            
    	float reward = problemMethods::getReward(cost.getSalesCost(),state,demand);
            
        State stateAfterDemandSatisfaction(location);
    	stateAfterDemandSatisfaction=state;
            
        problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
        
    	float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
        
    	float valueNextPeriod=0;
        
    	valueNextPeriod=valueFunction->getValueFunction(currentPeriod+1, hashFunction->hash_value(stateAfterDemandSatisfaction));
        
            
    	nextPeriodProfit+=((float)1/demandNumber)*(reward-holdingCost+valueNextPeriod);
            
    }
        
    if (expectedProfitNextPeriodsWithTransshipments>nextPeriodProfit) {
        return (expectedProfitNextPeriodsWithTransshipments);
    }
    else
        return nextPeriodProfit;
    
}


void ADPForwardOneTransshipment_Solver::initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction)
{
    F=NoTransshipmentFunction;
}

float ADPForwardOneTransshipment_Solver::getTransshipmentCost(const Cost& cost,const int& currentPeriod, const State& currentState, Actions& maxTransshipmentMatrix)const
{
    
    LP_optimizer lp_Problem;
    //maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, PVF->getIndexVector(0), PVF->getBreakpoints(0), PVF->getSlope(0));
    maxTransshipmentMatrix = lp_Problem.optimize(currentState,0, cost, F,currentPeriod);
    
    return lp_Problem.optimalObjectiveFunction();
    
}