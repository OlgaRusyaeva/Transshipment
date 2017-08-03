//
//  LookaheadMultyPeriods_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 01/03/15.
//
//

#include "LookaheadMultyPeriods_Simulator.h"
#include "PiecewiseLinearFunctionSet.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "MyopicMethods.h"

LookaheadMultyPeriods_Simulator::~LookaheadMultyPeriods_Simulator()
{
    delete F;
}


void LookaheadMultyPeriods_Simulator::initializeNoTransshipmentFunction(const PiecewiseLinearFunctionSet& noTransshipmentF)
{
    F=new PiecewiseLinearFunctionSet(noTransshipmentF.getSizeH(),noTransshipmentF.getSizeW());
    *F=noTransshipmentF;
}

float LookaheadMultyPeriods_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    
    bool do_Transshipment=true;
    
    vector_t left_over;
    vector_t sourceVector=myopicMethods::getLocationsForTransshipments(currentState);
    
    row_t profit_NoTransshipments(location);
    row_t profit_OneUnitMore(location);
    row_t profit_OneUnitLess(location);
    
    State stateOnePlus(location);
    State stateOneMinus(location);
    
    while (do_Transshipment)
    {
        left_over=myopicMethods::getNonStockOutLocations(currentState,sourceVector);
        
        if ((int)left_over.size()!=0)
        {
            /*cout<<"Left overs:"<<'\n';
             for (int i=0; i<(int)left_over.size(); i++) {
             cout<<left_over[i]<<'\n';
             }*/
            
            myopicMethods::setOnePlusState(currentState,stateOnePlus);
            myopicMethods::setOneMinusState(currentState,stateOneMinus,left_over);
            
            /*cout<<"stateOnePlus:"<<'\n';
             for (int i=0; i<state.getLength(); i++) {
             cout<<stateOnePlus.getElement(i)<<'\n';
             }
             
             cout<<"stateOneMinus:"<<'\n';
             for (int i=0; i<state.getLength(); i++) {
             cout<<stateOneMinus.getElement(i)<<'\n';
             }*/
            profit_NoTransshipments=getTransshipmentProfitRestPeriods(currentState, currentPeriod);
            profit_OneUnitMore=getTransshipmentProfitRestPeriods(stateOnePlus, currentPeriod);
            profit_OneUnitLess=getTransshipmentProfitRestPeriods(stateOneMinus, currentPeriod);
            
            
            /*cout<<"profit_NoTransshipments:"<<'\n';
             for (int i=0; i<state.getLength(); i++) {
             cout<<profit_NoTransshipments[i]<<'\n';
             }
             
             
             cout<<"profit_OneUnitMore:"<<'\n';
             for (int i=0; i<state.getLength(); i++) {
             cout<<profit_OneUnitMore[i]<<'\n';
             }
             
             cout<<"profit_OneUnitLess:"<<'\n';
             for (int i=0; i<state.getLength(); i++) {
             cout<<profit_OneUnitLess[i]<<'\n';
             }*/
            
            
            int k=myopicMethods::getLowestDecreaseInProfit(profit_NoTransshipments,profit_OneUnitLess,left_over);
            int j=myopicMethods::getHighestIncreaseInProfit(profit_NoTransshipments,profit_OneUnitMore,sourceVector);
            
            //cout<<"k: "<<k<<'\n';
            //cout<<"j: "<<j<<'\n';
            
            
            if ((k!=j)&&(profit_OneUnitMore[j]-profit_NoTransshipments[j])-(profit_NoTransshipments[k]-profit_OneUnitLess[k])>cost.getTransshipmentCost()[k]*cost.getDistance(k,j))
            {
                maxTransshipmentMatrix.setElement(k,j,maxTransshipmentMatrix.getElement(k,j)+1);
                currentState.setElement(k,0,currentState.getElement(k,0)-1);
                currentState.setElement(j,0,currentState.getElement(j,0)+1);
            }
            else
                do_Transshipment=false;
            
        }
        else
            do_Transshipment=false;
    }
    
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}

row_t LookaheadMultyPeriods_Simulator::getTransshipmentProfitRestPeriods(const State& currentState, const int& currentPeriod)
{
    const int L=currentState.getLength();
    row_t profit(L);
    for (int i=0; i<L; i++) {
        float expectedProfitNextPeriodsWithoutTransshipments=0;
        for (int j=0; j<F->get(currentPeriod,i).getBreakpointClosestTo(currentState.getElement(i,0)); j++) {
            //check
            expectedProfitNextPeriodsWithoutTransshipments+=F->get(currentPeriod,i).getSlope(F->get(currentPeriod,i).getBreakpoint(j));
        }
        profit[i]=expectedProfitNextPeriodsWithoutTransshipments;
    }
    
    return profit;
}
