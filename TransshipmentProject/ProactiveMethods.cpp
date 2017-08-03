//
//  ProactiveMethods.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ProactiveMethods.h"

float gettransshipmentCostByTBA(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, const int& currentPeriod,Actions& maxTransshipmentMatrix)
{
    
    vector_t lowerBound = calculateLowerBound(distribution,location);
    
    vector_t upperBound = calculateUpperBound(distribution,time-currentPeriod-1,location);
    
    vector_t stock_out = getShortageLocations(state,lowerBound);
    vector_t left_over = getExcessLocations(state,upperBound);
    
    
    if (((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) 
    { 
        
        list_t requiredQuantity = calculateRequiredQuantity(stock_out,state,upperBound);
        list_t availableQuantity = calculateAvailableQuantity(left_over,state,lowerBound);
        
        while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
            
            requiredQuantity.sort(MyDataSortPredicateDescending);
            availableQuantity.sort(MyDataSortPredicateDescending);
            
            int amountToTransship = getMinValue(availableQuantity.front().second, requiredQuantity.front().second);
            state.setElement(requiredQuantity.front().first, state.getElement(requiredQuantity.front().first)+amountToTransship);
            state.setElement(availableQuantity.front().first, state.getElement(availableQuantity.front().first)-amountToTransship);
            maxTransshipmentMatrix.setElement(availableQuantity.front().first,requiredQuantity.front().first,amountToTransship);
            
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
        
    }
        
    float transshipmentCost = getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}


float gettransshipmentCostByTBA2(const int& location, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, const int& currentPeriod, const vector_t& maxDemandForDistribution,Actions& maxTransshipmentMatrix)
{
    
    vector_t stock_out = getShortageLocations(state,maxDemandForDistribution);
    vector_t left_over = getExcessLocations(state,maxDemandForDistribution);
    
    if (((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) 
    {  

        list_t requiredQuantity = calculateRequiredQuantity(stock_out,state,maxDemandForDistribution);
        list_t availableQuantity = calculateAvailableQuantity(left_over,state,maxDemandForDistribution);
        
        while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
            
            requiredQuantity.sort(MyDataSortPredicateDescending);
            
            row_t distanceToFirstShortageElement=distanceMatrix.getElementsForState(requiredQuantity.front().first);
            sortAvailableQuantity(availableQuantity,distanceToFirstShortageElement);
            
            int amountToTransship = getMinValue(availableQuantity.front().second, requiredQuantity.front().second);
            state.setElement(requiredQuantity.front().first, state.getElement(requiredQuantity.front().first)+amountToTransship);
            state.setElement(availableQuantity.front().first, state.getElement(availableQuantity.front().first)-amountToTransship);
            maxTransshipmentMatrix.setElement(availableQuantity.front().first,requiredQuantity.front().first,amountToTransship);
            
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
    }
        
    float transshipmentCost = getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}

float gettransshipmentCostByTIE(const int& location, State& state, const Cost& cost, const Actions& distanceMatrix, vector<boost::shared_ptr<iDistribution> > distribution, const int& numberOfTransshipments,Actions& maxTransshipmentMatrix)
{
    
    vector_t bound = getBound(state,distribution,location);
    
    vector_t stock_out = getShortageLocations(state,bound);
    vector_t left_over = getExcessLocations(state,bound);
    
    if ((numberOfTransshipments==0)&&((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) 
    {   
        
        list_t requiredQuantity = calculateRequiredQuantity(stock_out,state,bound);
        list_t availableQuantity = calculateAvailableQuantity(left_over,state,bound);
        
        while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
            
            requiredQuantity.sort(MyDataSortPredicateDescending);
            availableQuantity.sort(MyDataSortPredicateDescending);
            
            int amountToTransship = getMinValue(availableQuantity.front().second, requiredQuantity.front().second);
            //new state after making a transshipment
            state.setElement(requiredQuantity.front().first, state.getElement(requiredQuantity.front().first)+amountToTransship);
            state.setElement(availableQuantity.front().first, state.getElement(availableQuantity.front().first)-amountToTransship);
            maxTransshipmentMatrix.setElement(availableQuantity.front().first,requiredQuantity.front().first,amountToTransship);
            
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
    }
    float transshipmentCost = getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}


/*--------------------------------help methods------------------------------------------------*/

vector_t calculateLowerBound(vector<boost::shared_ptr<iDistribution> > distribution,const int& numberOfLocations)
{
    vector_t lowerBound = vector_t(numberOfLocations);
    for(int i=0; i<numberOfLocations; i++)
    {
        lowerBound[i]=(int)distribution[i]->mean();
    }
    
    return lowerBound;
    
}


vector_t calculateUpperBound(vector<boost::shared_ptr<iDistribution> > distribution,const int& time, const int& numberOfLocations)
{
    vector_t upperBound = vector_t(numberOfLocations);
    for (int i=0; i<numberOfLocations; i++) {
        upperBound[i]=(int)(distribution[i]->mean())*time;
    }
    
    return upperBound;
}

vector_t getBound(const State& state,vector<boost::shared_ptr<iDistribution> > distribution,const int& numberOfLocations)
{
    vector_t bound = vector_t(numberOfLocations);
    int sumMean=0;
    int sumIL=0;
    for (int i=0; i<numberOfLocations; i++) {
        sumMean+=distribution[i]->mean();
        sumIL+=state.getElement(i);
    }
    
    for (int i=0; i<numberOfLocations; i++) {
        bound[i]=(int)(sumIL*(distribution[i]->mean())/sumMean);
    }
    
    return bound;
}






