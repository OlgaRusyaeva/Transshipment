//
//  TBA_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "TBA_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "NumbersOperations.h"
#include "RangingMethods.h"
#include "ProblemMethods.h"
#include "iProblem.h"

using namespace std;
using namespace numbersOperations;
using namespace rangingMethods;
    
        
 float TBA_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    int time = problem.getTimeHorizon();
    Cost cost = problem.getCost();
    
        vector_t lowerBound = calculateLowerBound(distribution,location);
        
        vector_t upperBound = calculateUpperBound(distribution,time-currentPeriod,location);
        
        vector_t stock_out = getShortageLocations(currentState,lowerBound);
        vector_t left_over = getExcessLocations(currentState,upperBound);
        
        
        if (((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) 
        { 
            
            list_t requiredQuantity = calculateRequiredQuantity(stock_out,currentState,upperBound);
            list_t availableQuantity = calculateAvailableQuantity(left_over,currentState,lowerBound);
            
            while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
                
                requiredQuantity.sort(MyDataSortPredicateDescending);
                availableQuantity.sort(MyDataSortPredicateDescending);
                
                int amountToTransship = getMinValue(availableQuantity.front().second, requiredQuantity.front().second);
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
        
    float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}
    
    
    
/*--------------------------------help methods------------------------------------------------*/
    
vector_t TBA_Simulator::calculateLowerBound(const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& numberOfLocations)
{
        vector_t lowerBound = vector_t(numberOfLocations);
        for(int i=0; i<numberOfLocations; i++)
        {
            lowerBound[i]=(int)distribution[i]->mean();
        }
        
        return lowerBound;
        
}
    
    
vector_t TBA_Simulator::calculateUpperBound(const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& time, const int& numberOfLocations)
{
        vector_t upperBound = vector_t(numberOfLocations);
        for (int i=0; i<numberOfLocations; i++) {
            upperBound[i]=(int)(distribution[i]->mean())*time;
        }
        
        return upperBound;
}
    
