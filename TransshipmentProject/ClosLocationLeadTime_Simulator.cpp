//
//  ClosLocationLeadTime_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 24/09/15.
//
//

#include "ClosLocationLeadTime_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "VectorsOperations.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "LeadTimes.h"

using namespace std;
using namespace vectorsOperations;

//transship 1/shareToTransship part of the stock
float ClosLocationLeadTime_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    LeadTimes leadTimes =problem.getLeadTimes();
    
    State oldState=currentState;
    for (int i=0; i<location; i++) {
        oldState.setElement(i,0,0);
    }
    problemMethods::getNewState(oldState);
    
    State newState(location,leadTimes.getMaxLeadTimePlusOne());
    for (int i=0; i<location; i++) {
        newState.setElement(i,0,currentState.getElement(i,0));
    }
    
    vector_t stock_out=getStockOutLocations(newState);
    
    //locations that can not be used to transship from
    vector_t closeTo_stock_out=getCloseToStockOutLocations(newState,shareToTransship);
    
    
    vector_t merge_stock_out=merge_copy(stock_out, closeTo_stock_out);
    
    int i=0;
    while (((int)merge_stock_out.size()!=location)&&((int)stock_out.size()!=i))
    {
        int j=0;
        j = getClosestLocation(cost.getDistanceFromLocation(stock_out[i]),merge_stock_out);
        int amountToTransship = getHowMuchToTransship(newState.getElement(j,0),shareToTransship);
        //new state after making a transshipment
        newState.setElement(stock_out[i],leadTimes.getLeadTimeBetweenLocations(j,stock_out[i]),amountToTransship);
        newState.setElement(j,0,newState.getElement(j,0)-amountToTransship);
        
        maxTransshipmentMatrix.setElement(j,stock_out[i],amountToTransship);
        
        closeTo_stock_out=getCloseToStockOutLocations(newState,shareToTransship);
        merge_stock_out=copy_no_sort(merge_copy(stock_out, closeTo_stock_out));
        i++;
        
        
    }
    
    mergeStates(oldState,newState);
    currentState=newState;
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}

State ClosLocationLeadTime_Simulator::getInitialState(iProblem& problem,const int& iteration) 
{
    return *initialState;
}

/*--------------------------------------help methods-----------------------------------*/

void ClosLocationLeadTime_Simulator::mergeStates(const State& oldState, State& newState)
{
    int L=newState.getLength();
    int D=newState.getDepth();
    for (int i=0; i<L; i++) {
        for (int t=0; t<D; t++) {
            newState.setElement(i,t,newState.getElement(i,t)+oldState.getElement(i,t));
        }
    }
}

//get Location with the lowest transshipment cost for the stock_out Location
int ClosLocationLeadTime_Simulator::getClosestLocation(row_t distanceVector, vector_t vectorOfStock_outLocations)
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
int ClosLocationLeadTime_Simulator::getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider)
{
    return floor((float)(stockOfDonorLocation/ divider));//transship the half of the donor stock
}

vector_t ClosLocationLeadTime_Simulator::getCloseToStockOutLocations(const State& state, const int& parameter)
{
    vector_t close_to_stock_out;
    for (int i=0; i<=state.getLength()-1; i++)
    {
        if ((state.getElement(i,0)<parameter)&&(state.getElement(i,0)!=0))
            close_to_stock_out.push_back(i);
    }
    return close_to_stock_out;
    
}

vector_t ClosLocationLeadTime_Simulator::getStockOutLocations(const State& initialState)
{
    const int L=initialState.getLength();
    vector_t stock_out;
    for (int i=0; i<=L-1; i++)
    {
        if (initialState.getElement(i,0)==0)
            stock_out.push_back(i);
    }
    return stock_out;
    
}
