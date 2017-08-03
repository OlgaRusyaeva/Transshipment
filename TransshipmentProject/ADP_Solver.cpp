//
//  ADP_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/7/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ADP_Solver.h"
#include "iDistribution.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "LeadTimes.h"
#include "MyopicMethods.h"
#include "iProblem.h"
#include "PLValueFunction.h"
#include "NumbersOperations.h"

//#include <math.h>

using namespace numbersOperations;

boost::shared_ptr<PLValueFunction>& ADP_Solver::getPiecewiseApp_ValueFunction()
{
    return approxVF;
}

void ADP_Solver::initializePiecewiseApp_ValueFunction(const int& location, const int& time, const int& lifeTime)
{
    approxVF = boost::shared_ptr<PLValueFunction>(new PLValueFunction(time,location,lifeTime));
}

void ADP_Solver::solve(iProblem& problem, const int& iterationNumber)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    int LT = problem.getLifeTime();
    
    initializePiecewiseApp_ValueFunction(L,T,LT);
        
    State initialState = problem.getState();
    Cost costInState = problem.getCost();
    Demand demand=problem.getDemand();
    LeadTimes leadTimes=problem.getLeadTimes();

    getApproximation(L,T,LT,initialState, costInState, demand, leadTimes, iterationNumber);
    
    printPLValueFunction();
    
}

void ADP_Solver::initializeLastPeriodApproximation(const int& time, const State& state, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{

        const int L=state.getLength();
        for (int i=0; i<L; i++) {
            int k=0;
            for (int j=0; j<=state.getElement(i,0); j++) {
                k++;
                float lastPeriodProfitCurrentIL=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-myopicMethods::expectedValuePositivePart(1,distribution[i],j))-cost.getHoldingCost()[i]*j;
                float lastPeriodProfitILOnePlus=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-myopicMethods::expectedValuePositivePart(1,distribution[i],(j+1)))-cost.getHoldingCost()[i]*(j+1);
                float value=lastPeriodProfitILOnePlus - lastPeriodProfitCurrentIL;
                if (j>0) {
                    approxVF->get(time,i,0).addBreakpoint(approxVF->get(time,i,0).getBreakpoint(j-1)+1);
                    approxVF->get(time,i,0).addSlope(value);
                }
                if (j==0) {
                    approxVF->get(time,i,0).setSlope(j,value);
                }
                
            }
            approxVF->get(time,i,0).setIndex(k);
        }
        
}
    

void ADP_Solver::getProfitOnePlus(row_t& profit,const State& state, const int& lifeTime, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();

    for (int i=0; i<L; i++) {
        int sumInventory=0;
        for (int m=0; m<lifeTime; m++) {
            sumInventory+=state.getElement(i,m);
        }
        if (sumInventory<demand.getElement(i)) {
            profit[i]=salesPrice[i];
        }
        else
            profit[i]=-holdingCost[i];//-
    }
}

void ADP_Solver::getProfitOneMinus(row_t& profit,const State& state, const int& lifeTime, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();
    
    for (int i=0; i<L; i++) {
        int sumInventory=0;
        for (int m=0; m<lifeTime; m++) {
            sumInventory+=state.getElement(i,m);
        }
        if (sumInventory>demand.getElement(i)) {
            profit[i]=-holdingCost[i];//-
        }
        else
            profit[i]=salesPrice[i];
    }

        
}
    
float ADP_Solver::getProfit(const int& IL, const int& d, const float& h, const float& p)
{
        return p*getMinValue(IL,d)-h*getMaxValue(IL-d,0);
}

void ADP_Solver::printPLValueFunction()
{
    int T=approxVF->getSizeH();
    int L=approxVF->getSizeW();
    int maxLeadTime=approxVF->getSizeD();
    
    for (int t=0; t<T;t++){
     	std::cout<<"Transshipment period: "<<t<<'\n';
     
     	std::cout<<"Index vector:"<<'\n';
     	for (int i=0; i<L; i++) {
     		for (int l=0; l<maxLeadTime; l++) {
     			std::cout<<"Index vector of location "<<i<<" lead time "<<l<<":"<<approxVF->get(t,i,l).getIndex()<<'\n';
    		}
    	 }
     	std::cout<<"Breakpoints:"<<'\n';
     	for (int i=0; i<L; i++) {
     		for (int l=0; l<maxLeadTime; l++) {
     			std::cout<<"Breakpoints of location: "<<i<<" lead time "<<l<<":";
     			for (int j=0; j<approxVF->get(t,i,l).getIndex(); j++) {
     				std::cout<<approxVF->get(t,i,l).getBreakpoint(j)<<" ";
     			}
     			std::cout<<'\n';
    		}
     	}
     	std::cout<<"Slopes:"<<'\n';
     	for (int i=0; i<L; i++) {
     		for (int l=0; l<maxLeadTime; l++) {
     			std::cout<<"SLopes of location: "<<i<<" lead time "<<l<<":";
     			for (int j=0; j<approxVF->get(t,i,l).getIndex(); j++) {
     				std::cout<<approxVF->get(t,i,l).getSlope(j)<<" ";
     			}
     			std::cout<<'\n';
    		}
     	}
     
	}
}
