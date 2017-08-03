//
//  ClosestLoc_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ClosestLoc_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "VectorsOperations.h"
#include "ProblemMethods.h"
#include "iProblem.h"

using namespace std;
using namespace vectorsOperations;
    
//transship 1/shareToTransship part of the stock
 float ClosestLoc_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{	
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    
    	vector_t stock_out=getStockOutLocations(currentState);
        
    	//locations that can not be used to transship from
        vector_t closeTo_stock_out=getCloseToStockOutLocations(currentState,shareToTransship);
        
        
        vector_t merge_stock_out=merge_copy(stock_out, closeTo_stock_out);
        
        int i=0;
        while (((int)merge_stock_out.size()!=location)&&((int)stock_out.size()!=i))
        {
            int j=0;
            j = getClosestLocation(cost.getDistanceFromLocation(stock_out[i]),merge_stock_out); 
            int amountToTransship = getHowMuchToTransship(currentState.getElement(j,0),shareToTransship);
            //new state after making a transshipment
            currentState.setElement(stock_out[i],0,amountToTransship);
            currentState.setElement(j,0,currentState.getElement(j,0)-amountToTransship);

            maxTransshipmentMatrix.setElement(j,stock_out[i],amountToTransship);
            
            closeTo_stock_out=getCloseToStockOutLocations(currentState,shareToTransship);
            merge_stock_out=copy_no_sort(merge_copy(stock_out, closeTo_stock_out));
            i++;
            
            
        }
        
    	float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}
    
    /*--------------------------------------help methods-----------------------------------*/

 
//get Location with the lowest transshipment cost for the stock_out Location
int ClosestLoc_Simulator::getClosestLocation(row_t distanceVector, vector_t vectorOfStock_outLocations) 
{
        
        for (int i=0; i<=(int)vectorOfStock_outLocations.size()-1; i++) {
            distanceVector[vectorOfStock_outLocations[i]]=0;
        }
        int indexClosestLocation=0;
        
        while (distanceVector[indexClosestLocation]==0) {
            indexClosestLocation++;
        }
        
        
        for (int i=0; i<=(int)distanceVector.size()-1; i++) {
            if((distanceVector[i]<distanceVector[indexClosestLocation])&&(distanceVector[i]!=0))
                indexClosestLocation=i;
        }
        return indexClosestLocation;
} 
    
//get how many items to transship from the donor Location
int ClosestLoc_Simulator::getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider)
{
        return floor((float)(stockOfDonorLocation/ divider));//transship the half of the donor stock
}
    
vector_t ClosestLoc_Simulator::getCloseToStockOutLocations(const State& state, const int& parameter)
{
        vector_t close_to_stock_out;
        for (int i=0; i<=state.getLength()-1; i++) 
        {
            if ((state.getElement(i,0)<parameter)&&(state.getElement(i,0)!=0))
                close_to_stock_out.push_back(i);
        }
        return close_to_stock_out;
        
}
    