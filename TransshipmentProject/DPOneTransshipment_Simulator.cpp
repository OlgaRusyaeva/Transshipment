//
//  DPOneTransshipmentPeriod_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#include "DPOneTransshipment_Simulator.h"
#include "iProblem.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ActionsGenerator.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"
#include "Demand.h"
#include "ValueFunction.h"
#include "Hashing.h"

using namespace std;
using namespace actions;
using namespace arrays;

bool DPOneTransshipment_Simulator::timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)
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
    
    float expectedProfitNextPeriodWithTransshipments=getExpectedProfitNextPeriodsWithTransshipments(location,cost,demand, currentPeriod, currentState, currentIteration, distribution);
    
    if((!isZero)&&(expectedProfitNextPeriodsWithCurrentTransshipment>=expectedProfitNextPeriodWithTransshipments))
    {
        flag=true;
    }
    
    
    return flag;

}

float DPOneTransshipment_Simulator::getExpectedProfitNextPeriodsWithTransshipments(const int& location,const Cost& cost, Demand& demand,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    
    float nextPeriodProfit=0;
    State minBorder(location);
    int size_demands=1;
    for (int i=0; i<location; i++) {
        minBorder.setElement(i,0,currentState.getElement(i,0));
        size_demands*=currentState.getElement(i,0)+1;
    }
    Array2D<float> possibleDemands(location,size_demands);
    possibleDemands.initialize();
    simulationMethods::generatePossibleDemands(minBorder,possibleDemands);
    
    for (int i=0; i<possibleDemands.sizeH(); i++)
    {
        //calculate probability
        float prob=1;
        bool flag;
        for (int j=0; j<location; j++) {
            flag=true;
            demand.setElement(j, (int)possibleDemands(i,j));
            if ((currentState.getElement(j,0)-demand.getElement(j))==0) {
                flag=false;
            }
            prob*=problemMethods::probabilityOfDemand(demand.getElement(j),distribution[j],flag);
        }
        
        //get reward
        float reward = problemMethods::getReward(cost.getSalesCost(),currentState,demand);
        
        State stateAfterDemandSatisfaction(location);
        stateAfterDemandSatisfaction=currentState;
        
        problemMethods::getNewState(stateAfterDemandSatisfaction, demand);//!!!
        
        //get holding cost
        float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
        //cout<<"Holding cost:"<<holdingCost<<'\n';
        
        float valueNextPeriod=0;
        
        valueNextPeriod=VF->getValueFunction(currentPeriod+1, HF->hash_value(stateAfterDemandSatisfaction));
        
        nextPeriodProfit+=prob*(reward-holdingCost+valueNextPeriod);
        
    }
    return nextPeriodProfit;
}

void DPOneTransshipment_Simulator::initializeValueFunction(const ValueFunction& valueFunction)
{
    VF=new ValueFunction();
    *VF=valueFunction;
    
}

void DPOneTransshipment_Simulator::initializeHashFunction(const Hashing& hashFunction)
{
    HF=new Hashing();
    *HF=hashFunction;
}

DPOneTransshipment_Simulator::~DPOneTransshipment_Simulator()
{
    delete VF;
    delete HF;
}