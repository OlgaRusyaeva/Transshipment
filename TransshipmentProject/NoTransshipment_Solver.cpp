//
//  ADP_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/7/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "NoTransshipment_Solver.h"
#include "iDistribution.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "MyopicMethods.h"
#include "randomNumber.h"

using namespace problemMethods;

boost::shared_ptr<PiecewiseLinearFunctionSet> NoTransshipment_Solver::getExpectedProfitWithoutTransshipments() const
{
    return PVF;
}

void NoTransshipment_Solver::initialize(const int& location, const int& time)
{
    PVF = boost::shared_ptr<PiecewiseLinearFunctionSet>(new PiecewiseLinearFunctionSet(time,location));
    UB = boost::shared_ptr<State>(new State(location));
}

void NoTransshipment_Solver::solve(iProblem& problem)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    initialize(L,T);
    
    State initialState = problem.getState();
    Cost cost = problem.getCost();
    Demand demand=problem.getDemand();
    matrixInt_t identical=problem.getIndenticalLocations();
    
    for (int i=0; i<L; i++) {
        UB->setElement(i,0,initialState.getElement(i,0));
    }
    
    //constructValueFunctionForRestPeriods(T,  L, cost, initialState, demand,identical);
    
    //setUpperBound(cost);
    

    
    
}


void NoTransshipment_Solver::constructValueFunctionForRestPeriods(const int& time, const int& location, const Cost& cost, const State& currentState,  Demand& demand, const matrixInt_t& identical)
{
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,0);
    
    int aggregatedIL=0;
    for (int i=0; i<location; i++) {
        aggregatedIL+=currentState.getElement(i,0);
    }
    
    //TODO upper bound
    vector_t upperBound(location);
    for (int i=0; i<location; i++) {
        //upperBound[i]=currentState.getElement(i)+floor(0.7*aggregatedIL);
        upperBound[i]=aggregatedIL;
    }
    
    initializeLastPeriodValueFunction(time-1,location,upperBound,cost,distribution,identical);
    
    //identical is used to avoid additional computation for idetical locations
    //in case of non-identical locations the parallelism should be applied - TODO
    for (int t=time-2; t>=0; t--) {
        for (int i=0; i<location; i++) {
            if (identical[i].empty()!=true) {
                int index=identical[i][0];

            	for (int j=0; j<upperBound[index]+1; j++) {
                	float value=probabilityOfDemand(0,distribution[index],true)*(PVF->get(t+1,index).getSlope(j)-cost.getHoldingCost()[index])+probabilityOfDemand(j+1,distribution[index],false)*cost.getSalesCost()[index];
                	for (int d=1; d<j+1; d++) {
                    	value+=probabilityOfDemand(d,distribution[index],true)*(PVF->get(t+1,index).getSlope(j-d)-cost.getHoldingCost()[index]);
                    }
                    for (int k=0; k<identical[i].size(); k++) {
                        int jndex=identical[i][k];
                        PVF->get(t,jndex).setIndex(upperBound[jndex]+1);//including zero
                		if (j!=0) {
                    		PVF->get(t,jndex).addBreakpoint(j);
                    		PVF->get(t,jndex).addSlope(value);
                		}
						else
                    		PVF->get(t,jndex).setSlope(j,value);
                    }
            	}
            }
        }
    }
    
       
}
    

void NoTransshipment_Solver::initializeLastPeriodValueFunction(const int& t, const int L, const vector_t& upperBound, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution,const matrixInt_t& identical)
{
    
    for (int i=0; i<L; i++) {
        if (identical[i].empty()!=true) {
            int index=identical[i][0];
            for (int j=0; j<distribution[index]->appMaxValue()+10; j++) {
                float lastPeriodProfitCurrentIL=(cost.getSalesCost()[index]+cost.getHoldingCost()[index])*(distribution[index]->mean()-myopicMethods::expectedValuePositivePart(1,distribution[index],j))-cost.getHoldingCost()[index]*j;
                float lastPeriodProfitILOnePlus=(cost.getSalesCost()[index]+cost.getHoldingCost()[index])*(distribution[index]->mean()-myopicMethods::expectedValuePositivePart(1,distribution[index],(j+1)))-cost.getHoldingCost()[index]*(j+1);
                float value=lastPeriodProfitILOnePlus - lastPeriodProfitCurrentIL;
                
                for (int k=0; k<identical[i].size(); k++) {
                    int jndex=identical[i][k];
                    PVF->get(t,jndex).setIndex(upperBound[jndex]+1);//including zero
                    if (j>0) {
                        PVF->get(t,jndex).addBreakpoint(j);
                        PVF->get(t,jndex).addSlope(value);
                    }
                    if (j==0) {
                        PVF->get(t,jndex).setSlope(j,value);
                    }
                }
                
            }
            for (int j=distribution[i]->appMaxValue()+10; j<=upperBound[i]; j++) {
                
                for (int k=0; k<identical[i].size(); k++) {
                    int jndex=identical[i][k];
                    PVF->get(t,jndex).setIndex(upperBound[jndex]+1);//including zero
                    if (j>0) {
                        PVF->get(t,jndex).addBreakpoint(j);
                        PVF->get(t,jndex).addSlope(-cost.getHoldingCost()[jndex]);
                    }
                    if (j==0) {
                        PVF->get(t,jndex).setSlope(j,-cost.getHoldingCost()[jndex]);
                    }
                }
            }
        }
        
    }
    
}
void NoTransshipment_Solver::getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();
    for (int i=0; i<L; i++) {
        if (state.getElement(i,0)<demand.getElement(i)) {
            profit[i]=salesPrice[i];
        }
        else
            profit[i]=-holdingCost[i];//-
    }
}

void NoTransshipment_Solver::getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();
    for (int i=0; i<L; i++) {
        if (state.getElement(i,0)>demand.getElement(i)) {
            profit[i]=-holdingCost[i];//-
        }
        else
            profit[i]=salesPrice[i];
    }
    
    
}

boost::shared_ptr<State> NoTransshipment_Solver::getUpperBound()const
{
    return UB;
}

void NoTransshipment_Solver::setUpperBound(const Cost& cost)
{
    for (int i=0; i<UB->getLength(); i++) {
        UB->setElement(i,0,PVF->get(0,i).getOptimalBreakpoint(cost.getOrderingCost()[i]));
    }
}

