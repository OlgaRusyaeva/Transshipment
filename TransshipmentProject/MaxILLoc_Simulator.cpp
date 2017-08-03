//
//  MaxILLoc_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "MaxILLoc_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "VectorsOperations.h"
#include "ProblemMethods.h"
#include "iProblem.h"

using namespace std;
using namespace vectorsOperations;

    
 float MaxILLoc_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    
        vector_t stock_out=getStockOutLocations(currentState);
        
        vector_t closeTo_stock_out=getCloseToStockOutLocations(currentState,shareToTransship);
        
        
        vector_t merge_stock_out=merge_copy(stock_out, closeTo_stock_out);
        
        State oldState(location);
        oldState=currentState;
        
        int i=0;
        while (((int)merge_stock_out.size()!=location)&&((int)stock_out.size()!=i))
        {
            int j=0;
            j=getLocationWithHighestInventoryLevel(oldState,merge_stock_out);
            int amountToTransship = getHowMuchToTransship(currentState.getElement(j,0),shareToTransship);
            //new state after making a transshipment
            currentState.setElement(stock_out[i], 0,amountToTransship);
            currentState.setElement(j, 0, currentState.getElement(j,0)-amountToTransship);
            maxTransshipmentMatrix.setElement(j,stock_out[i],amountToTransship);
            
            closeTo_stock_out=getCloseToStockOutLocations(currentState,shareToTransship);
            merge_stock_out=copy_no_sort(merge_copy(stock_out, closeTo_stock_out));
            i++;
            
            
        }
        
    float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}
    
/*--------------------------------------help methods-----------------------------------*/

//get Location with the highest inventory level for the stock_out Location
int MaxILLoc_Simulator::getLocationWithHighestInventoryLevel(const State& state,vector_t vectorOfStock_outLocations)
{
        int indexRichestLocation=0;
        for (int i=0; i<=state.getLength()-1; i++) {
            if ((state.getElement(i,0)>state.getElement(indexRichestLocation,0))&&(find(vectorOfStock_outLocations.begin(), vectorOfStock_outLocations.end(), i)==vectorOfStock_outLocations.end()))
            {
                indexRichestLocation=i;
            }
        }
        return indexRichestLocation;
}
    
//get how many items to transship from the donor Location
int MaxILLoc_Simulator::getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider)
{
        return floor((float)(stockOfDonorLocation/ divider));//transship the half of the donor stock
}
    
vector_t MaxILLoc_Simulator::getCloseToStockOutLocations(const State& state, const int& parameter)
{
        vector_t close_to_stock_out;
        for (int i=0; i<=state.getLength()-1; i++) 
        {
            if ((state.getElement(i,0)<parameter)&&(state.getElement(i,0)!=0))
                close_to_stock_out.push_back(i);
        }
        return close_to_stock_out;
        
}
    
    