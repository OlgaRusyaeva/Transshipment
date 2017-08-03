//
//  ExpectedProfitApproximation_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 9/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

//
//  ADP_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/7/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//



//-------------------------------TEST--BEFORE--USING--------------------------------------
#include "ExpectedProfitApproximation_Solver.h"
#include "PiecewiseApp_ValueFunction.h"
#include "iDistribution.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "NumbersOperations.h"
#include "MyopicMethods.h"
#include "randomNumber.h"
#include <math.h> 

using namespace numbersOperations;

PiecewiseApp_ValueFunction& ExpectedProfitApproximation_Solver::getPiecewiseApp_ValueFunction() const
{
    return *approxVF;
}

void ExpectedProfitApproximation_Solver::initializePiecewiseApp_ValueFunction(const int& location, const int& time)
{
    approxVF = new PiecewiseApp_ValueFunction(time,location);
}

void ExpectedProfitApproximation_Solver::solve(iProblem& problem, const int& iterationNumber)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    
    initializePiecewiseApp_ValueFunction(L,T);
    
    State initialState = problem.getState();
    Cost costInState = problem.getCost();
    Demand demand=problem.getDemand();
    
    
    getApproximation(L,T,initialState, costInState, demand, iterationNumber);
    
    
}


void ExpectedProfitApproximation_Solver::update(const int& t, const int& loc,  const float& new_slopePlus, const float& new_slopeMinus, const int& pos_A, const int& pos_B, const int& currentState)
{
    for (int i=pos_A; i<pos_B; i++) {
        if (approxVF->getBreakpointsElement(t,loc,i)<currentState) {
            approxVF->setSlopeElement(t,loc,i, (1-alpha)*approxVF->getSlopeElement(t,loc,i)+alpha*new_slopeMinus);
        }
        else
            approxVF->setSlopeElement(t,loc,i, (1-alpha)*approxVF->getSlopeElement(t,loc,i)+alpha*new_slopePlus);
    }
}

void ExpectedProfitApproximation_Solver::insert(const int& t, const int& loc, const int& new_breakpoint, int& increase_index, int& pos)
{
    
    int k=approxVF->getIndexVectorElement(t,loc);
    row_t u=approxVF->getBreakpointsRow(t,loc);
    row_t v=approxVF->getSlopeRow(t,loc);
    
    for (int i=0; i<k+increase_index; i++) {
        if (u[i]==new_breakpoint) {
            pos=i;
            break;
        }
        else
        {
            if (u[i]>new_breakpoint) {
                approxVF->insertBreakpointsElement(t,loc,i,new_breakpoint);
                approxVF->insertSlopeElement(t,loc,i,v[i-1]);
                pos=i;
                increase_index++;
                break;
                
            }
            else
            {
                if (i==(k+increase_index)-1) {
                    approxVF->push_backBreakpointsElement(t,loc,new_breakpoint);
                    approxVF->push_backSlopeElement(t,loc,v[i]);
                    pos=i+1;
                    increase_index++;
                    break;
                }
            }
        }
    }
}




//should change k, u, v
void ExpectedProfitApproximation_Solver::updateSlope(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& L, const int& iteration)
{
    int k=approxVF->getIndexVectorElement(t,loc);
    row_t u=approxVF->getBreakpointsRow(t,loc);
    row_t v=approxVF->getSlopeRow(t,loc);
    
    alpha=(float)5/(5+iteration); 
    
    int left=0;
    int right=0;
    
    int increment=0;
    int pos_a=0;
    int pos_b=0;
    int pos_c=0;
    
    //find min_k where the concave property will be broken
    //if the left side of min update interval<0 then pos_a=0
    if (inventory-step>=0) {
        
        int min_k=-1;
        
        for (int j=0; j<k; j++) {
            if (j==k-1)
            {
                //if we add one additional breakpoint at the end
                //we need to check that v[k-1]>=(1-alpha)v[k-1]+alha*gradientMinus
                if (v[j]<=gradientMinus) {
                    min_k=j;
                    break;
                }
                
            }
            else
            {
                if (v[j]<=(1-alpha)*v[j+1]+alpha*gradientMinus) {
                    min_k=j;
                    break;
                }
            }
        }
        
        //if min_k doesn't exist OR min{inventory-step,u[min_k]}=invnetory-step
        if ((min_k<0)||(inventory-step<u[min_k])) {
            //take the left side of min update interval
            left=inventory-step;
        }
        else
        {
            //take the left side the breakpoint of min_k
            left=u[min_k];
        }
        
        //insert breakpoint if it is required and define the left position for the update
        insert(t,loc, left,increment, pos_a);
        
    }
    
    //find max_k where the concave property will be broken
    int max_k=-1;
    
    for (int j=k-1; j>=0; j--) {
        if (j==0) 
        {
            if (v[j]>=gradientPlus) {
                max_k=j;
                break;
            }
            
        }
        else
        {
            if ((1-alpha)*v[j-1]+alpha*gradientPlus<=v[j]) {
                max_k=j;
                break;
            }
        }
    }
    
    if (max_k+1>=k) {
        right=-1;
    }
    else
    {
        //define the right side of the update interval
        if ((max_k<0)||(inventory+step>u[max_k+1])) {
            right=inventory+step;
        }
        else
        {
            right=u[max_k+1];
        }
    }
    
    //insert breakpoint if it is required
    insert(t,loc, inventory,increment,pos_b);
    
    //insert breakpoint if it is required and define right position for the update interval
    if (right>0) {
        insert(t,loc, right,increment,pos_c);
    }
    else
        pos_c=k+increment;
    
    update(t,loc, gradientPlus, gradientMinus, pos_a, pos_c, inventory);
    
    approxVF->setIndexVectorElement(t,loc,k+increment);
    
}


void ExpectedProfitApproximation_Solver::getApproximation(const int& location, const int& time, State& initialState,const  Cost& cost,  Demand& demand, const int& iterationNumber)
{
    State state(location);
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,20);
    
    randomDoubleNumber randomNumberGenerator(0,1);
    std::vector<boost::shared_ptr<randomIntNumber> > randomILGenerator;
    
    
    for (int n=0; n <iterationNumber;n++)
    {
        
        state=initialState;
        
        State preDemandState=state;
        
        for (int t=0; t<time; t++) 
        { 
            
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
            }
            
            problemMethods::getNewState(state, demand);
            
            row_t profitWhatIfOneMore(location);
            row_t profitWhatIfOneLess(location);
                
            getProfitOnePlus(profitWhatIfOneMore,preDemandState,demand, cost.getHoldingCost(), cost.getSalesCost());
            getProfitOneMinus(profitWhatIfOneLess,preDemandState,demand, cost.getHoldingCost(), cost.getSalesCost());
                
                for (int i=0; i<location; i++) {
                    
                    if (preDemandState.getElement(i)<demand.getElement(i)) {
                        updateSlope(t,i, profitWhatIfOneMore[i], profitWhatIfOneLess[i], preDemandState.getElement(i),location,n);
                    }
                    else
                    {
                        if (preDemandState.getElement(i)>demand.getElement(i)) {
                            updateSlope(t,i, profitWhatIfOneMore[i]+approxVF->getSlopeElement(t+1,i,preDemandState.getElement(i)-demand.getElement(i)+1), profitWhatIfOneLess[i]+approxVF->getSlopeElement(t+1,i,preDemandState.getElement(i)-demand.getElement(i)-1), preDemandState.getElement(i),location,n);
                        }
                        else
                            updateSlope(t,i, profitWhatIfOneMore[i]+approxVF->getSlopeElement(t+1,i,1), profitWhatIfOneLess[i], preDemandState.getElement(i),location,n);
                    }
                    
                    
                }
            
            
        }
        
    }
    
}




void ExpectedProfitApproximation_Solver::getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();
    for (int i=0; i<L; i++) {
        if (state.getElement(i)<demand.getElement(i)) {
            profit[i]=salesPrice[i];
        }
        else
            profit[i]=-holdingCost[i];//-
    }
}

void ExpectedProfitApproximation_Solver::getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();
    for (int i=0; i<L; i++) {
        if (state.getElement(i)>demand.getElement(i)) {
            profit[i]=-holdingCost[i];//-
        }
        else
            profit[i]=salesPrice[i];
    }
    
    
}

float ExpectedProfitApproximation_Solver::getProfit(const int& IL, const int& d, const float& h, const float& p)
{
    return p*getMinValue(IL,d)-h*getMaxValue(IL-d,0);
}



