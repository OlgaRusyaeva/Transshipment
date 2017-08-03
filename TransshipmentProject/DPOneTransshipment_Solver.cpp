//
//  DPOneTransshipment_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#include "DPOneTransshipment_Solver.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "PiecewiseLinearFunctionSet.h"
#include "LP_optimizer.h"
#include "Actions.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"

using namespace std;
using namespace arrays;


float DPOneTransshipment_Solver::getMaxProfit(const int& t, const int& time, const State& state, Demand& demand, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution) const
{
    
    const int location=state.getLength();
    
    Actions transshipmentMatrix(location);
    
    float expectedProfitNextPeriodsWithTransshipments=getTransshipmentCost(cost, t, state, transshipmentMatrix);
    
    float nextPeriodProfit=0;
    
    /*for (int i=0; i<location; i++) {
        cout<<state.getElement(i)<<",";
    }*/
    
    State minBorder(location);
    int size_demands=1;
    for (int i=0; i<location; i++) {
        minBorder.setElement(i,0,state.getElement(i,0));
        size_demands*=state.getElement(i,0)+1;
    }
    //Array2D<float> possibleDemands(location,size_demands);
    //possibleDemands.initialize();
    //simulationMethods::generatePossibleDemands(minBorder,possibleDemands);
    
    State inv(location);
    int i=0;
    while (i<size_demands) {
		
        inv=simulationMethods::generateInventoryLevel(minBorder.getCurrentPeriodState().begin(),i,location,1);
        
        //calculate probability
        float prob=1;
        bool flag;
        for (int j=0; j<location; j++) {
            flag=true;
            demand.setElement(j, inv.getElement(j,0));
            if ((state.getElement(j,0)-demand.getElement(j))==0) {
                flag=false;
            }
            prob*=problemMethods::probabilityOfDemand(demand.getElement(j),distribution[j],flag);
        }

        //get reward
        float reward = problemMethods::getReward(cost.getSalesCost(),state,demand);
        
        State stateAfterDemandSatisfaction(location);
        stateAfterDemandSatisfaction=state;
        
        problemMethods::getNewState(stateAfterDemandSatisfaction, demand);//!!!
        
        //get holding cost
        float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
        //cout<<"Holding cost:"<<holdingCost<<'\n';
        
        float valueNextPeriod=0;
        
        valueNextPeriod=valueFunction->getValueFunction(t+1, hashFunction->hash_value(stateAfterDemandSatisfaction));
        
        //cout<<"value next period: "<<valueNextPeriod<<'\n';
        //get value function
        
        nextPeriodProfit+=prob*(reward-holdingCost+valueNextPeriod);
        
    }
    
    //cout<<nextPeriodProfit<<'\n';
    
    if (expectedProfitNextPeriodsWithTransshipments>nextPeriodProfit) {
        return (expectedProfitNextPeriodsWithTransshipments);
    }
    else
        return nextPeriodProfit;
    
}


void DPOneTransshipment_Solver::initializePiecewiseAppVF(const  boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction)
{
    F=NoTransshipmentFunction;
}

float DPOneTransshipment_Solver::getTransshipmentCost(const Cost& cost,const int& currentPeriod, const State& currentState, Actions& maxTransshipmentMatrix)const
{
    
    LP_optimizer lp_Problem;
    //maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, PVF->getIndexVector(0), PVF->getBreakpoints(0), PVF->getSlope(0));
    maxTransshipmentMatrix = lp_Problem.optimize(currentState, 0,cost, F,currentPeriod);
    
    return lp_Problem.optimalObjectiveFunction();
    
}


/*float DPOneTransshipment_Solver::getExpectedProfitNextPeriodsWithoutTransshipments(const int& location,const int& currentPeriod, const State& currentState)const
{
    
    float expectedProfitNextPeriodsWithoutTransshipments=0;
    for (int i=0; i<location; i++) {
        for (int j=0; j<F->getBreakpointIndexClosestToElement(currentPeriod,i,currentState.getElement(i)); j++) {
            //check
            expectedProfitNextPeriodsWithoutTransshipments+=F->getSlopeElement(currentPeriod,i,F->getBreakpointsElement(currentPeriod,i,j));
        }
    }
    
    return expectedProfitNextPeriodsWithoutTransshipments;

}*/
