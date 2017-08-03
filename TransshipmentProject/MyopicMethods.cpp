//
//  MyopicMethods.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "MyopicMethods.h"
#include "iDistribution.h"
#include "State.h"
#include "Cost.h"
#include "ProblemMethods.h"
#include <assert.h> 
#include "omp.h"


namespace myopicMethods {
    /*float expectedValuePositivePart(const boost::shared_ptr<iDistribution>& distr,const int& stateValue)
    {
        assert(stateValue>=0);
        if (stateValue==0) {
            return (distr->mean());
        }
        else
        {
            return (expectedValuePositivePart(distr,stateValue-1)-distr->complement_cdf(stateValue-1));
        }
    }*/
    
    float expectedValuePositivePart(const int& time,const boost::shared_ptr<iDistribution>& distr,const int& stateValue)
    {
        assert(stateValue>=0);
        float exp=0;
//#pragma omp parallel for reduction(-:exp)
        for (int i=stateValue-1; i>=0; i--) {
            exp-=1-distr->cdfSum(i,time);//cdfSumIndependetRandomVariables(time,distr,i);//distr->cdf(i)
        }
        return (exp+time*distr->mean());
    }
    
    /*float cdfSumIndependetRandomVariables(const int& amount,const boost::shared_ptr<iDistribution>& distr,const int& x)
    {
        float cdf=0;
        if (x>amount*distr->maxValue()) {
            cdf=1;
        }
        else
        {
            if (x<amount*distr->minValue()) {
                cdf=0;
            }
            else
            {
                for (int i=amount*distr->minValue(); i<=x; i++) {
                    cdf+=pdfSumIndependetRandomVariables(i,distr,amount);
                }
            }

        }
        
        return cdf;
    }
    
    float pdfSumIndependetRandomVariables(const int& x,const boost::shared_ptr<iDistribution>& distr,int amount)
    {
        float pdf=0;
        
        if (amount==1) {
            pdf=distr->pdf(x);
        }
        else
            for (int i=distr->minValue(); i<=x; i++) {
                pdf+=distr->pdf(x-i)*pdfSumIndependetRandomVariables(i,distr,amount-1);
            }
        
        return pdf;
    }*/
    
    vector_t getNonStockOutLocations(const State& state, const vector_t& sourceVector)
    {
        const int L=(int)sourceVector.size();
        vector_t non_stockout;
        for (int i=0; i<L; i++) {
            if (state.getElement(sourceVector[i],0)>0) {
                non_stockout.push_back(sourceVector[i]);
            }
        }
        return non_stockout;
    }
    
    void setOnePlusState(const State& state,State& stateOnePlus)
    {
        const int L=state.getLength();
        for (int i=0; i<L; i++) {
            stateOnePlus.setElement(i,0, state.getElement(i,0)+1);
        }
    }
    
    void setOneMinusState(const State& state,State& stateOneMinus,const vector_t& left_over)
    {
        const int size=(int)left_over.size();
        for (int i=0; i<size; i++) {
            stateOneMinus.setElement(left_over[i],0, state.getElement(left_over[i],0)-1);
        }
        
    }
    
    row_t getTransshipmentProfitRestPeriods(const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const State& state,const int& restTime)
    {
        const int L=state.getLength();
        row_t profit(L);
        for (int i=0; i<L; i++) {
            profit[i]=getExpectedProfitRestPeriods(cost.getSalesCost()[i],cost.getHoldingCost()[i],state.getElement(i,0),distribution[i],restTime);
        }
        return profit;
    }
    
    int getLowestDecreaseInProfit(const row_t& profitNo, const row_t& profitLess, const vector_t& left_over)
    {
        const int size=(int)left_over.size();
        int k=left_over[0];
        float lowestDecrease=(float)(profitNo[k]-profitLess[k]);
        for (int i=1; i<size; i++) {
            if(lowestDecrease>profitNo[left_over[i]]-profitLess[left_over[i]])
            {
                lowestDecrease=profitNo[left_over[i]]-profitLess[left_over[i]];
                k=left_over[i];
            }
        }
        return k;
    }
    
    int getHighestIncreaseInProfit(const row_t& profitNo, const row_t& profitMore, const vector_t& destinationVector)
    {
        const int L=(int)destinationVector.size();
        int j=destinationVector[0];
        float highestIncrease=(float)(profitMore[j]-profitNo[j]);
        for (int i=1; i<L; i++) 
        {
            if (highestIncrease<profitMore[destinationVector[i]]-profitNo[destinationVector[i]]) 
            {
                highestIncrease=profitMore[destinationVector[i]]-profitNo[destinationVector[i]];
                j=destinationVector[i];
            }
        }
        return j;
        
    }
    
    float getExpectedProfitRestPeriods(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr, const int& restTime)
    {
        return ((p+h)*(restTime*distr->mean()-expectedValuePositivePart(restTime,distr,IL))-h*IL);
        //return (p+restTime*h/2)*(restTime*distr->mean()-expectedValuePositivePart(restTime,distr,IL));//average inventory to hold
        
        /*int currentIL=IL;
        int nextIL=IL-distr->mean()+expectedValuePositivePart(distr,IL);
        float profit=onePeriodProfit(currentIL,nextIL,p,h);
        for (int t=1; t<restTime; t++) {
            currentIL=nextIL;
            nextIL=IL-distr->mean()+expectedValuePositivePart(distr,IL);
            profit+=onePeriodProfit(currentIL,nextIL,p,h);
        }
        return profit;*/
    }
    
    float onePeriodProfit(const int& currentIL, const int& nextIL, const float& p, const float& h)
    {
        return p*currentIL-(p+h)*nextIL;
    }
    
    vector_t getLocationsForTransshipments(const State& state)
    {
        vector_t transshipmentLocations(state.getLength());
        for (int i=0; i<state.getLength(); i++) {
            transshipmentLocations[i]=i;
        }
        return transshipmentLocations;
    }
    

}

