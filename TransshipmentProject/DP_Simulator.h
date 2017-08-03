//
//  DP_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_DP_Simulator_h
#define TransshipmentProject_DP_Simulator_h

#include "Simulator.h"
#include "types.h"


class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class Hashing;
class ValueFunction;
class iProblem;

class DP_Simulator: public Simulator
{
public:
    DP_Simulator(const int& numberOfIterations, const int& seedNumber,const ValueFunction& valueFunction,const Hashing& hashFunction):Simulator(numberOfIterations,seedNumber){initializeValueFunction(valueFunction);initializeHashFunction(hashFunction);}
    ~DP_Simulator();
    
     float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
private:
    ValueFunction *VF;
    Hashing *HF;
    
    void initializeValueFunction(const ValueFunction& valueFunction);
    
    void initializeHashFunction(const Hashing& hashFunction);
    
    
    
};




#endif
