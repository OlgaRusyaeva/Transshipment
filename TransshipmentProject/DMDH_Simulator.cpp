//
//  DMDH_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#include "DMDH_Simulator.h"
#include "iProblem.h"
#include "State.h"
#include "PiecewiseLinearFunctionSet.h"
#include "Actions.h"
#include "Cost.h"
#include "Demand.h"
#include "iDistribution.h"
#include "ProblemMethods.h"
#include <boost/thread.hpp>
#include <boost/thread/detail/thread_group.hpp>


using namespace boost;

bool DMDH_Simulator::timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)
{
    bool flag=false;
    bool isZero=false;
    
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    State state=currentState;
    Demand demand=problem.getDemand();
    
    if (currentPeriod==0)
        dampingFactor=1.01;
    else
        dampingFactor=1;
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed+currentPeriod+20);
    
    float expectedProfitNextPeriodsWithCurrentTransshipment=getExpectedProfitWithCurrentTransshipment(cost, currentPeriod, state, isZero);
    
	if(!isZero)
    {
        float expectedProfitNextPeriodWithTransshipments=getExpectedProfitNextPeriodWithTransshipment(problem, currentPeriod, currentState, currentIteration, distribution);
        
        if(expectedProfitNextPeriodsWithCurrentTransshipment>=(float)dampingFactor*expectedProfitNextPeriodWithTransshipments)
        {
        	flag=true;
        }
    }
    
    
    return flag;
}

float DMDH_Simulator::getExpectedProfitNextPeriodWithTransshipment(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    int location = problem.getStateDimension();
    int threadCallNumber=rn/threadNumber;
    Cost cost = problem.getCost();
    Demand demand=problem.getDemand();
    
    //do it based on the approximation
    if (currentPeriod==problem.getTimeHorizon()-1) {
        float expectedProfitLastPeriod=getExpectedProfitNextPeriodsWithoutTransshipments(location,currentPeriod,currentState);
        //for (int i=0; i<location; i++) {
        //expectedProfitLastPeriod+=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-expectedValuePositivePart(1,distribution[i],currentState.getElement(i)))-cost.getHoldingCost()[i]*currentState.getElement(i);
        // }
        return expectedProfitLastPeriod;
    }
    else
    {
        row_t expectedProfit(rn);
        //Right way: to do it for any possible demand realization
        boost::thread_group groupOfThreads;
        for (int th=0; th<threadCallNumber; th++) {
            for (int r=0; r<threadNumber; r++) {

            	groupOfThreads.create_thread(boost::bind(&DMDH_Simulator::getExpectedProfitNextPeriodForFixedDemand,this,location,cost,demand,currentPeriod,currentState,distribution,boost::ref(expectedProfit[r+threadNumber*th])));

            }
            groupOfThreads.join_all();

        }

        float sum=0;
        for (int i=0; i<rn; i++) {
            sum+=expectedProfit[i];
        }
        return sum/rn;
    }
    
}


void DMDH_Simulator::getExpectedProfitNextPeriodForFixedDemand(const int& location, const Cost& cost, Demand& demand,const int& currentPeriod, const State& currentState,const std::vector<boost::shared_ptr<iDistribution> >& distribution, float& expectedProfit)
{


    bool isZero=false;
    
    for (int i=0; i<location; i++) {
        demand.setElement(i, (int)distribution[i]->sample());
    }

    float reward = problemMethods::getReward(cost.getSalesCost(),currentState,demand);

    State stateAfterDemandSatisfaction=currentState;
    
    problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
    

    float holdingCost = problemMethods::getCost(cost.getHoldingCost(),stateAfterDemandSatisfaction);

    float expectedProfitNextPeriodsWithCurrentTransshipment=getExpectedProfitWithCurrentTransshipment(cost, currentPeriod+1, stateAfterDemandSatisfaction,isZero);

    expectedProfit=reward-holdingCost+expectedProfitNextPeriodsWithCurrentTransshipment;

}

void DMDH_Simulator::setThreadNumber(const int& replicationNumber)
{
    int maxThreadNumber=boost::thread::hardware_concurrency();
    if (replicationNumber%maxThreadNumber>0) {
        threadNumber=5;
        if (replicationNumber%threadNumber>0) {
            std::cout<<"Warning: the replication number should be divisible by the thread number"<<'\n';
        }
    }
    else
   	 	threadNumber=maxThreadNumber;
}