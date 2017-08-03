//
//  ADPOneTransshipment_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 05/03/15.
//
//

#include "ADPOneTransshipment_Simulator.h"
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

bool ADPOneTransshipment_Simulator::timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)
{
    bool flag=false;
    bool isZero=false;;
    
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

float ADPOneTransshipment_Simulator::getExpectedProfitNextPeriodsWithTransshipments(const int& location,const Cost& cost, Demand& demand,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    
    float nextPeriodProfit=0;
    
    for (int i=0; i<demandNumber; i++)
    {
        for (int j=0; j<location; j++) {
            demand.setElement(j, (int)distribution[j]->sample());
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
        
        //cout<<"value next period: "<<valueNextPeriod<<'\n';
        //get value function
        
        nextPeriodProfit+=((float)1/demandNumber)*(reward-holdingCost+valueNextPeriod);
        
    }
    return nextPeriodProfit;
}

void ADPOneTransshipment_Simulator::initializeValueFunction(const ValueFunction& valueFunction)
{
    VF=new ValueFunction();
    *VF=valueFunction;
    
}

void ADPOneTransshipment_Simulator::initializeHashFunction(const Hashing& hashFunction)
{
    HF=new Hashing();
    *HF=hashFunction;
}

ADPOneTransshipment_Simulator::~ADPOneTransshipment_Simulator()
{
    delete VF;
    delete HF;
}