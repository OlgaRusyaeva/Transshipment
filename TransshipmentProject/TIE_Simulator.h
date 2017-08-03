//
//  TIE_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_TIE_Simulator_h
#define TransshipmentProject_TIE_Simulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class iProblem;

class TIE_Simulator: public Simulator
{
public:
    TIE_Simulator(const int& numberOfIterations, const int& seedNumber):Simulator(numberOfIterations,seedNumber){}
    ~TIE_Simulator(){}
    
     float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
    
    
private:
    
    /*--------------------------------help methods------------------------------------------------*/
    
    vector_t getBound(const State& state,const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& numberOfLocations);
    
    bool transshipmentOrderPoint(const State& state,const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& numberOfLocations);
    
    void sortAvailableQuantity(list_t& availableQuantity,row_t& distanceToFirstShortageElement);
    
};


#endif
