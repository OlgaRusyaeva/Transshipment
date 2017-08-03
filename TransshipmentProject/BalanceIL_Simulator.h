//
//  BalanceIL_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 9/25/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_BalanceIL_Simulator_h
#define TransshipmentProject_BalanceIL_Simulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Cost;
class Actions;
class Demand;
class iProblem;


class BalanceIL_Simulator: public Simulator
{
public:
    BalanceIL_Simulator(const int& numberOfIterations, const int& seedNumber):Simulator(numberOfIterations,seedNumber){}
    ~BalanceIL_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
private:
    vector_t getBound(const State& currentState,const int& numberOfLocations);
    
    void sortAvailableQuantity(list_t& availableQuantity,row_t& distanceToFirstShortageElement);
    
    
    
};

#endif
