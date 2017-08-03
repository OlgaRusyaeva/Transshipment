//
//  MaxRunOutLoc_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_MaxRunOutLoc_Simulator_h
#define TransshipmentProject_MaxRunOutLoc_Simulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class iProblem;

class MaxRunOutLoc_Simulator: public Simulator
{
public:
    MaxRunOutLoc_Simulator(const int& numberOfIterations, const int& seedNumber):Simulator(numberOfIterations,seedNumber){shareToTransship=3;}
    ~MaxRunOutLoc_Simulator(){}
    
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
    
private:
     int shareToTransship;
    /*--------------------------------------help methods-----------------------------------*/
    
    //get how many items to transship from the donor Location
    int getHowMuchToTransship(const int& stockOfDonorLocation, const int& divider);
    
    int getMaxRunOutTimeLocation(const State& state,const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& time,vector_t vectorOfStock_outLocations);
    
    int getRunOutTime(const int& state, const float& expectedDemand, const int& time);
    
    vector_t getCloseToStockOutLocations(const State& state, const int& parameter);
    
    
};

#endif
