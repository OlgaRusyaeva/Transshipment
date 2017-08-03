//
//  Pooling.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 4/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Pooling.h"

row_t solveByPooling(const int& location, const int& time, const long& numberOfIterations,State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
{
    row_t profit=row_t(numberOfIterations);
    
    int numberOfClusters = (int)(2);
    const vector_t& vectorOfClusters=Cluster(location,distanceMatrix,numberOfClusters);
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    const vector_t& maxDemandForDistribution = getMaxDemandForDistribution(distribution,location);
    
    for (int n=0; n <numberOfIterations;n++)
    {
        //cout<<"-------n-------"<<n<<'\n';
        float totalProfit = 0;
        State state(location);
        state.setState(initialState);
        
        for (int t=0; t<time; t++) 
        {
            //cout<<"Demand:"<<'\n';
            Demand demand(location);
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
               // cout<<demand->getElement(i)<<'\n';
            }
        
            float reward = getReward(cost.getSalesCost(),state,demand); 
            
            state.setState(getNewState(state, demand));
            
            float holdingCost = getHoldingCost(cost.getHoldingCost(),state);
            
            //delete demand;
            
            if ((t!=time-1))
            {  
                totalProfit+=reward-holdingCost;
                
                for (int i=0; i<numberOfClusters; i++) {
                    vector_t locationsOfOneCluster;
                    for (int k=0; k<location; k++) {
                        if (vectorOfClusters[k]==i) {
                            locationsOfOneCluster.push_back(k);
                        }

                    }
                    vector_t stock_out;
                    vector_t left_over;
                    for (int j=0; j<(int)locationsOfOneCluster.size(); j++) {
                        if (state.getElement(j)<=maxDemandForDistribution[j]) {
                            stock_out.push_back(j);
                        }
                        else
                            left_over.push_back(j);
                    }
                    
                    Actions transshipmentMatrix(location);// = initializeTransshipmentMatrix(location,location);
                    
                    list_t requiredQuantity = calculateRequiredQuantity(stock_out,state,maxDemandForDistribution);
                    list_t availableQuantity = calculateAvailableQuantity(left_over,state,maxDemandForDistribution);
                    while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
                        
                        requiredQuantity.sort(MyDataSortPredicateDescending);
                        
                        row_t distanceToFirstShortageElement=distanceMatrix.getElementsForState(requiredQuantity.front().first);
                        sortAvailableQuantity(availableQuantity,distanceToFirstShortageElement);
                        
                        int amountToTransship = getMinValue(availableQuantity.front().second, requiredQuantity.front().second);
                        state.setElement(requiredQuantity.front().first, state.getElement(requiredQuantity.front().first)+amountToTransship);
                        state.setElement(availableQuantity.front().first, state.getElement(availableQuantity.front().first)-amountToTransship);
                        transshipmentMatrix.setElement(availableQuantity.front().first,requiredQuantity.front().first,amountToTransship);
                        
                        if ((amountToTransship!=availableQuantity.front().second)||(amountToTransship!=requiredQuantity.front().second)) {
                            if (amountToTransship!=availableQuantity.front().second) {
                                //add available - amount to availableQuantity
                                availableQuantity.push_back(make_pair(availableQuantity.front().first, availableQuantity.front().second-amountToTransship));
                            }
                            else {
                                // add required - amount to requiredQuantity
                                requiredQuantity.push_back(make_pair(requiredQuantity.front().first, requiredQuantity.front().second-amountToTransship));
                            }
                        }
                        requiredQuantity.pop_front();
                        availableQuantity.pop_front(); 
                        
                    }
                    
                    float transshipmentCost = getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,transshipmentMatrix);
                    
                    //cout<<"transshipment matrix:"<<'\n';
                    //printMatrix(location, location, transshipmentMatrix);
                    totalProfit-=transshipmentCost;
                    
                    
                }
                
            }

            else
                totalProfit+=reward-holdingCost;//as a holding cost the same for all locations no sense to make transshipments on the last period
            
            
            
        }
        //delete state;
        profit[n]=totalProfit;
        
    }
    //std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );

    return profit;
    
}


static bool MyDataSortPredicateAscending(const pair<int,int>& lhs, const pair<int,int>& rhs) 
{ 
    return (lhs.second < rhs.second); 
} 

static bool MyDataSortPredicateDescending(const pair<int,int>& lhs, const pair<int,int>& rhs) 
{ 
    return (lhs.second > rhs.second); 
} 