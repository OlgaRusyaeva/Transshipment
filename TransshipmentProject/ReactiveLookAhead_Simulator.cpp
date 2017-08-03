//
//  ReactiveLookAhead_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/26/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ReactiveLookAhead_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "MyopicMethods.h"
#include "ProblemMethods.h"

using namespace std;
    
    
 float ReactiveLookAhead_Simulator::getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, Actions& maxTransshipmentMatrix)
{
        vector_t stock_out=getStockOutLocations(state);
		vector_t left_over=getNonStockOutLocations(state);

    int restTime=time-currentPeriod;
        
        int j=0;
        while (((int)stock_out.size()!=j)&&(stock_out.size()!=location)) 
        {
            
            /*cout<<"stock out"<<'\n';
            for (int i=0; i<(int)stock_out.size(); i++) {
                cout<<stock_out[i]<<'\n';
            }
            
            
            cout<<"left over"<<'\n';
            for (int i=0; i<(int)left_over.size(); i++) {
                cout<<left_over[i]<<'\n';
            }*/
            
            //left_over=getNonStockOutLocations(state);
            int l=stock_out[j];
            int k=l;
            int amountToTransship=0;
            float futureProfitDifference=-std::numeric_limits<float>::max();
            for (int i=0; i<(int)left_over.size(); i++) {
                int howMuch=0;
                int profitIJ=getFutureProfit(l,left_over[i],cost,distanceMatrix,state,distribution,howMuch,restTime);
                if (profitIJ>futureProfitDifference) {
                    futureProfitDifference=profitIJ;
                    k=left_over[i];
                    amountToTransship=howMuch;
                }
            }
            if (futureProfitDifference>=0) {
                maxTransshipmentMatrix.setElement(k,l,maxTransshipmentMatrix.getElement(k,l)+amountToTransship);
                state.setElement(k,state.getElement(k)-amountToTransship);
                state.setElement(l,state.getElement(l)+amountToTransship);
            }
			j++;
        }
        /*cout<<"Transshipment matrix:"<<'\n';
        for (int i=0; i<location; i++) {
            for (int j=0; j<location; j++) {
                cout<<maxTransshipmentMatrix.getElement(i,j)<<" ";
            }
            cout<<'\n';
        }*/
        
    float transshipmentCost = problemMethods::getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}
    
vector_t ReactiveLookAhead_Simulator::getNonStockOutLocations(const State& state)
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
    
//change calculations of future profit difference
float ReactiveLookAhead_Simulator::getFutureProfit(const int& j, const int& i,const Cost& cost, const Actions& distanceMatrix, const State& state, vector<boost::shared_ptr<iDistribution> > distribution, int& amountToTransship, const int& restTime)
{
        float profitDiff=0;
        float profitNoTransship=getAllPeriodsProfit(cost.getSalesCost()[i],cost.getHoldingCost()[i],state.getElement(i),distribution[i],restTime)+getAllPeriodsProfit(cost.getSalesCost()[j],cost.getHoldingCost()[j],state.getElement(j),distribution[j],restTime);
        float transshCost=cost.getTransshipmentCost()[i]*distanceMatrix.getElement(i,j);
        int y=1;
        
        while ((state.getElement(i)>=y)&&((getAllPeriodsProfit(cost.getSalesCost()[i],cost.getHoldingCost()[i],state.getElement(i)-y,distribution[i],restTime)+getAllPeriodsProfit(cost.getSalesCost()[j],cost.getHoldingCost()[j],state.getElement(j)+y,distribution[j],restTime)-y*transshCost-profitNoTransship-profitDiff)>0))
        {
            
            profitDiff=getAllPeriodsProfit(cost.getSalesCost()[i],cost.getHoldingCost()[i],state.getElement(i)-y,distribution[i],restTime)+getAllPeriodsProfit(cost.getSalesCost()[j],cost.getHoldingCost()[j],state.getElement(j)+y,distribution[j],restTime)-y*transshCost-profitNoTransship;
            amountToTransship=y;
            y++;
        }
        return profitDiff;
}
    
float ReactiveLookAhead_Simulator::getAllPeriodsProfit(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr,const int& time)
{
        float profit=0;
        int inventory=IL;
        for (int t=0; t<time; t++) {
            profit+=getOnePeriodProfit(p,h,inventory,distr);
            inventory=myopicMethods::expectedValuePositivePart(distr,inventory)+inventory-distr->mean();
        }
        return profit;
}
    
float ReactiveLookAhead_Simulator::getOnePeriodProfit(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr)
{
        return ((p+h)*(distr->mean()-myopicMethods::expectedValuePositivePart(distr,IL))-h*IL);
}


