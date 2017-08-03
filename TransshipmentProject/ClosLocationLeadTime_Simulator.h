//
//  ClosLocationLeadTime_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 24/09/15.
//
//

#ifndef __TransshipmentProject__ClosLocationLeadTime_Simulator__
#define __TransshipmentProject__ClosLocationLeadTime_Simulator__

#include "types.h"
#include "SimulatorReplenishmentAndTransshipments.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class iProblem;

class ClosLocationLeadTime_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    ClosLocationLeadTime_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<State>& upperBound):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber){shareToTransship=3;initialState=upperBound;}
    ~ClosLocationLeadTime_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration,Actions& maxTransshipmentMatrix);
    State getInitialState(iProblem& problem,const int& iteration);
    
private:
    boost::shared_ptr<State> initialState;
    
    int shareToTransship;
    /*--------------------------------------help methods-----------------------------------*/
    
    void mergeStates(const State& oldState, State& newState);
    
    //get Location with the lowest transshipment cost for the stock_out Location
    int getClosestLocation(row_t distanceVector, vector_t vectorOfStock_outLocations);
    
    //get how many items to transship from the donor Location
    int getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider);
    
    vector_t getCloseToStockOutLocations(const State& state, const int& parameter);
    
    vector_t getStockOutLocations(const State& initialState);
    
    
};

#endif /* defined(__TransshipmentProject__ClosLocationLeadTime_Simulator__) */
