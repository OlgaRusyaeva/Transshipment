//
//  LookAhead_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/25/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "LookAhead_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "MyopicMethods.h"
#include "ProblemMethods.h"

using namespace std;

    
 float LookAhead_Simulator::getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, Actions& maxTransshipmentMatrix)
{
        bool do_Transshipment=true;
        vector_t left_over;
        row_t profit_NoTransshipmentsOnePeriod(location);
        row_t profit_OneUnitMoreOnePeriod(location);
        row_t profit_OneUnitLessOnePeriod(location);
        
        row_t profit_NoTransshipmentsTwoPeriods(location);
        row_t profit_OneUnitMoreTwoPeriods(location);
        row_t profit_OneUnitLessTwoPeriods(location);
        
        State stateOnePlus(location);
        State stateOneMinus(location);
        
        while (do_Transshipment) 
        {
            left_over=getNonStockOutLocations(state);
            
            if ((int)left_over.size()!=0)
            { 
                
                setOnePlusState(state,stateOnePlus);
                setOneMinusState(state,stateOneMinus,left_over);
                
                profit_NoTransshipmentsTwoPeriods=getTransshipmentProfitTwoPeriods(cost, distribution, state);
                profit_OneUnitMoreTwoPeriods=getTransshipmentProfitTwoPeriods(cost, distribution, stateOnePlus);
                profit_OneUnitLessTwoPeriods=getTransshipmentProfitTwoPeriods(cost, distribution, stateOneMinus);
                
                profit_NoTransshipmentsOnePeriod=getTransshipmentProfitOnePeriod(cost, distribution, state);
                profit_OneUnitMoreOnePeriod=getTransshipmentProfitOnePeriod(cost, distribution, stateOnePlus);
                profit_OneUnitLessOnePeriod=getTransshipmentProfitOnePeriod(cost, distribution, stateOneMinus);
                
                cout<<"profit_NoTransshipments:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<profit_NoTransshipmentsTwoPeriods[i]<<'\n';
                    cout<<profit_NoTransshipmentsOnePeriod[i]<<'\n';
                }
                
                
                cout<<"profit_OneUnitMore:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<profit_OneUnitMoreTwoPeriods[i]<<'\n';
                    cout<<profit_OneUnitMoreOnePeriod[i]<<'\n';
                }
                
                cout<<"profit_OneUnitLess:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<profit_OneUnitLessTwoPeriods[i]<<'\n';
                    cout<<profit_OneUnitLessOnePeriod[i]<<'\n';
                }
                
                
                int k=getLowestDecreaseInProfit(profit_NoTransshipmentsTwoPeriods,profit_OneUnitLessTwoPeriods,state,left_over);
                int j=getHighestIncreaseInProfit(profit_NoTransshipmentsTwoPeriods,profit_OneUnitMoreTwoPeriods,state);
                
                cout<<"k: "<<k<<'\n';
                cout<<"j: "<<j<<'\n';
                
                if (((profit_OneUnitMoreTwoPeriods[j]-profit_NoTransshipmentsTwoPeriods[j])-(profit_NoTransshipmentsTwoPeriods[k]-profit_OneUnitLessTwoPeriods[k])>cost.getTransshipmentCost()[k]*distanceMatrix.getElement(k,j))&&(profit_OneUnitMoreOnePeriod[j]-profit_NoTransshipmentsOnePeriod[j])>=(profit_NoTransshipmentsOnePeriod[k]-profit_OneUnitLessOnePeriod[k])) 
                {
                    maxTransshipmentMatrix.setElement(k,j,maxTransshipmentMatrix.getElement(k,j)+1);
                    state.setElement(k,state.getElement(k)-1);
                    state.setElement(j,state.getElement(j)+1);
                }
                else
                {
                    /*int kk=getLowestDecreaseInProfit(profit_NoTransshipmentsOnePeriod,profit_OneUnitLessOnePeriod,state,left_over);
                    int jj=getHighestIncreaseInProfit(profit_NoTransshipmentsOnePeriod,profit_OneUnitMoreOnePeriod,state);
                    
                    if ((profit_OneUnitMoreOnePeriod[jj]-profit_NoTransshipmentsOnePeriod[jj])-(profit_NoTransshipmentsOnePeriod[kk]-profit_OneUnitLessOnePeriod[kk])>cost.getTransshipmentCost()[k]*distanceMatrix.getElement(kk,jj)) 
                    {
                        maxTransshipmentMatrix.setElement(kk,jj,maxTransshipmentMatrix.getElement(kk,jj)+1);
                        state.setElement(kk,state.getElement(kk)-1);
                        state.setElement(jj,state.getElement(jj)+1);
                    }
                    else*/
                        do_Transshipment=false;
                }
                                                                                                                                                                                              
                
            }
            else
                do_Transshipment=false;
        }
        
        
    float transshipmentCost = problemMethods::getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}
    
vector_t LookAhead_Simulator::getNonStockOutLocations(const State& state)
{
        const int L=state.getLength();
        vector_t non_stockout;
        for (int i=0; i<L; i++) {
            if (state.getElement(i)>0) {
                non_stockout.push_back(i);
            }
        }
        return non_stockout;
}
    
void LookAhead_Simulator::setOnePlusState(const State& state,State& stateOnePlus)
{
        const int L=state.getLength();
        for (int i=0; i<L; i++) {
            stateOnePlus.setElement(i,state.getElement(i)+1);
        }
}
    
void LookAhead_Simulator::setOneMinusState(const State& state,State& stateOneMinus,const vector_t& left_over)
{
        const int size=(int)left_over.size();
        for (int i=0; i<size; i++) {
            stateOneMinus.setElement(left_over[i],state.getElement(left_over[i])-1);
        }
        
}
    
row_t LookAhead_Simulator::getTransshipmentProfitOnePeriod(const Cost& cost, vector<boost::shared_ptr<iDistribution> > distribution, const State& state)
{
        const int L=state.getLength();
        row_t profit(L);
        for (int i=0; i<L; i++) {
            profit[i]=getOnePeriodProfit(cost.getSalesCost()[i],cost.getHoldingCost()[i],state.getElement(i),distribution[i]);
        }
        return profit;
}
    
row_t LookAhead_Simulator::getTransshipmentProfitTwoPeriods(const Cost& cost, vector<boost::shared_ptr<iDistribution> > distribution, const State& state)
{
        const int L=state.getLength();
        row_t profit(L);
        for (int i=0; i<L; i++) {
            profit[i]=getOnePeriodProfit(cost.getSalesCost()[i],cost.getHoldingCost()[i],state.getElement(i),distribution[i])+getOnePeriodProfit(cost.getSalesCost()[i],cost.getHoldingCost()[i],(myopicMethods::expectedValuePositivePart(distribution[i],state.getElement(i))+state.getElement(i)-distribution[i]->mean()),distribution[i]);
        }
        return profit;
}
    
int LookAhead_Simulator::getLowestDecreaseInProfit(const row_t& profitNo, const row_t& profitLess, const State& state,const vector_t& left_over)
{
        const int size=(int)left_over.size();
        int k=left_over[0];
        int lowestDecrease=profitNo[k]-profitLess[k];
        for (int i=1; i<size; i++) {
            if(lowestDecrease>profitNo[left_over[i]]-profitLess[left_over[i]])
            {
                lowestDecrease=profitNo[left_over[i]]-profitLess[left_over[i]];
                k=left_over[i];
            }
        }
        return k;
}
    
int LookAhead_Simulator::getHighestIncreaseInProfit(const row_t& profitNo, const row_t& profitMore, const State& state)
{
        const int L=state.getLength();
        int j=0;
        int highestIncrease=profitMore[j]-profitNo[j];
        for (int i=1; i<L; i++) 
        {
            if (highestIncrease<profitMore[i]-profitNo[i]) 
            {
                highestIncrease=profitMore[i]-profitNo[i];
                j=i;
            }
        }
        return j;
        
}
    
float LookAhead_Simulator::getOnePeriodProfit(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr)
{
    	cout<<"IL "<<IL<<'\n';
    	cout<<"p "<<p<<'\n';
        cout<<"h "<<h<<'\n';
    	cout<<"mean "<<distr->mean()<<'\n';
    	cout<<"(d-IL)+ "<<myopicMethods::expectedValuePositivePart(distr,IL)<<'\n';
        return ((p+h)*(distr->mean()-myopicMethods::expectedValuePositivePart(distr,IL))-h*IL);
}

