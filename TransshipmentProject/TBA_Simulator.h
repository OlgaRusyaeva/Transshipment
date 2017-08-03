//
//  TBA_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_TBA_Simulator_h
#define TransshipmentProject_TBA_Simulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class iProblem;

class TBA_Simulator: public Simulator
{
public:
    TBA_Simulator(const int& numberOfIterations, const int& seedNumber):Simulator(numberOfIterations,seedNumber){}
    ~TBA_Simulator(){}

    
     float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
    
    
    
private:
    
    /*--------------------------------help methods------------------------------------------------*/
    
    vector_t calculateLowerBound(const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& numberOfLocations);
    
    
    vector_t calculateUpperBound(const std::vector<boost::shared_ptr<iDistribution> >& distribution,const int& time, const int& numberOfLocations);
    
    
};




#endif
