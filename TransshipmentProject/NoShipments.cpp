//
//  NoShipments.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "NoShipments.h"

row_t solveByNoShipmentsMethod(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)//don't use distanceMatrix
{
    row_t profit=row_t(numberOfIterations);
    
    vector<iDistribution*> distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    for (int n=0; n <numberOfIterations;n++)
    {
       //cout<<"-------n-------"<<n<<'\n';
        float totalProfit = 0;
        State* state = new State(location);
        state->setState(initialState);
        for (int t=0; t<time; t++) 
        {
            //cout<<"Demand:"<<'\n';
            Demand *demand= new Demand(location);
            for (int i=0; i<location; i++) {
                demand->setElement(i, (int)distribution[i]->sample());
                //cout<<demand->getElement(i)<<'\n';
            }
            float reward = getReward(cost.getSalesCost(),*state,*demand); 
                        
            state->setState(getNewState(*state, *demand));
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(),*state);
            
            delete demand;

            totalProfit+=reward-holdingCost;
        }
        delete state;
        profit[n]=totalProfit;
    }
    
    std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    
    //return (float)totalProfit/(float)numberOfIterations;
    return getStatistics(profit);
    
}