//
//  BalanceIL_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "BalanceIL_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ProblemMethods.h"
#include "iDistribution.h"
#include "NumbersOperations.h"
#include "RangingMethods.h"
#include "iProblem.h"

using namespace std;
using namespace numbersOperations;
using namespace rangingMethods;


float BalanceIL_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    
    vector_t bound = getBound(currentState,location);
    
    vector_t stock_out = getShortageLocations(currentState,bound);
    vector_t left_over = getExcessLocations(currentState,bound);
    
        
    if (((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) 
    {   
            
            list_t requiredQuantity = calculateRequiredQuantity(stock_out,currentState,bound);
            list_t availableQuantity = calculateAvailableQuantity(left_over,currentState,bound);
            
            while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
                
                requiredQuantity.sort(MyDataSortPredicateDescending);
                //availableQuantity.sort(MyDataSortPredicateDescending);
                
                row_t distanceToFirstShortageElement=cost.getDistanceFromLocation(requiredQuantity.front().first);
                sortAvailableQuantity(availableQuantity,distanceToFirstShortageElement);
                
                int amountToTransship = getMinValue(availableQuantity.front().second, requiredQuantity.front().second);
                //new state after making a transshipment
                currentState.setElement(requiredQuantity.front().first, 0, currentState.getElement(requiredQuantity.front().first,0)+amountToTransship);
                currentState.setElement(availableQuantity.front().first, 0, currentState.getElement(availableQuantity.front().first,0)-amountToTransship);
                maxTransshipmentMatrix.setElement(availableQuantity.front().first,requiredQuantity.front().first,amountToTransship);
                
                if ((amountToTransship!=availableQuantity.front().second)||(amountToTransship!=requiredQuantity.front().second)) {
                    if (amountToTransship!=availableQuantity.front().second) {
                        //add available - amount to availableQuantity
                        availableQuantity.push_back(std::make_pair(availableQuantity.front().first, availableQuantity.front().second-amountToTransship));
                    }
                    else {
                        // add required - amount to requiredQuantity
                        requiredQuantity.push_back(std::make_pair(requiredQuantity.front().first, requiredQuantity.front().second-amountToTransship));
                    }
                }
                
                requiredQuantity.pop_front();
                availableQuantity.pop_front();  
                
            }  
    }

    
    float transshipmentCost =problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
    
    return transshipmentCost;

}

vector_t BalanceIL_Simulator::getBound(const State& currentState,const int& numberOfLocations)
{
    int sum=0;
    for (int i=0; i<numberOfLocations; i++) {
        sum+=currentState.getElement(i,0);
    }
    
    vector_t bound = vector_t(numberOfLocations);
    
    int balanceLevel=floor(sum/numberOfLocations);//ceil
    for (int i=0;i<numberOfLocations; i++) {
        bound[i]=balanceLevel;
    }
    //sum-balanceLevel*(numberOfLocations-1);

    return bound;
    
}

void BalanceIL_Simulator::sortAvailableQuantity(list_t& availableQuantity,row_t& distanceToFirstShortageElement)
{
    int j=availableQuantity.front().first;
    float quantity=availableQuantity.front().second;
    float distance=distanceToFirstShortageElement[j];
    
    for (list_t::const_iterator i=availableQuantity.begin(); i!=availableQuantity.end(); i++) {
        if (distanceToFirstShortageElement[(*i).first]<distance) {
            j=(*i).first;
            quantity=(*i).second;
            distance=distanceToFirstShortageElement[j];
        }
    }
    availableQuantity.remove(make_pair(j, quantity));
    availableQuantity.push_front(make_pair(j, quantity));
}

