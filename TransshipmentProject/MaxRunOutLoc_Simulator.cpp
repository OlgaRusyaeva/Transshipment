//
//  MaxRunOutLoc_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "MaxRunOutLoc_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "VectorsOperations.h"
#include "ProblemMethods.h"
#include "iProblem.h"

using namespace std;
using namespace vectorsOperations;

    
   
 float MaxRunOutLoc_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    int time = problem.getTimeHorizon();
    Cost cost = problem.getCost();
        
        vector_t stock_out=getStockOutLocations(currentState);
        
        vector_t closeTo_stock_out=getCloseToStockOutLocations(currentState,shareToTransship);
        
        
        vector_t merge_stock_out=merge_copy(stock_out, closeTo_stock_out);
        
        //how much and from where to transship
        // matrixInt_t transshipmentMatrix = initializeTransshipmentMatrix(location,location);
        
        State oldState(location);
        oldState=currentState;
        
        int i=0;
        while (((int)merge_stock_out.size()!=location)&&((int)stock_out.size()!=i))
        {
            int j=0;
            j=getMaxRunOutTimeLocation(oldState,distribution,time,merge_stock_out);
            int amountToTransship = getHowMuchToTransship(currentState.getElement(j,0),shareToTransship);
            //new state after making a transshipment
            currentState.setElement(stock_out[i], 0,amountToTransship);
            currentState.setElement(j, 0,currentState.getElement(j,0)-amountToTransship);
            maxTransshipmentMatrix.setElement(j,stock_out[i],amountToTransship);
            
            closeTo_stock_out=getCloseToStockOutLocations(currentState,shareToTransship);
            merge_stock_out=copy_no_sort(merge_copy(stock_out, closeTo_stock_out));
            i++;
            
            
        }
        
    float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}
    
    /*--------------------------------------help methods-----------------------------------*/
    
//get how many items to transship from the donor Location
int MaxRunOutLoc_Simulator::getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider)
{
        return floor((float)(stockOfDonorLocation/ divider));//transship the half of the donor stock
}
    
int MaxRunOutLoc_Simulator::getMaxRunOutTimeLocation(const State& state,const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& time,vector_t vectorOfStock_outLocations)
{
        int indexMaxRunOutTimeLocation=0;
        int runOutTime=-1;
        for (int i=0; i<=(int)state.getLength()-1;i++ ) {
            if ((getRunOutTime(state.getElement(i,0),distribution[i]->mean(),time)>runOutTime)&&(find(vectorOfStock_outLocations.begin(), vectorOfStock_outLocations.end(), i)==vectorOfStock_outLocations.end()))
            {
                indexMaxRunOutTimeLocation=i;
                runOutTime=getRunOutTime(state.getElement(indexMaxRunOutTimeLocation,0),distribution[indexMaxRunOutTimeLocation]->mean(),time);
            }
        }
        return indexMaxRunOutTimeLocation;    
}
    
int MaxRunOutLoc_Simulator::getRunOutTime(const int& state, const float& expectedDemand, const int& time)
{
        return (int)(state/(expectedDemand*time));
}
    
vector_t MaxRunOutLoc_Simulator::getCloseToStockOutLocations(const State& state, const int& parameter)
{
        vector_t close_to_stock_out;
        for (int i=0; i<=state.getLength()-1; i++) 
        {
            if ((state.getElement(i,0)<parameter)&&(state.getElement(i,0)!=0))
                close_to_stock_out.push_back(i);
        }
        return close_to_stock_out;
        
}
    
    