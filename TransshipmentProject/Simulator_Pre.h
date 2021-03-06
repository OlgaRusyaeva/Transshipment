//
//  Simulator_Pre-decisionState.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/25/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Simulator_Pre_decisionState_h
#define TransshipmentProject_Simulator_Pre_decisionState_h

#include"types.h"
#include "Simulator.h"

class iProblem;
class Profit;
class State;
class Actions;
class Demand;
class Cost;
class iDistribution;

class Simulator_Pre: public Simulator
{
public:
    
    Simulator_Pre(){}
    
    virtual ~Simulator_Pre(){}
    
    void simulator(const int& location, const int& time, const State& initialState,const Cost& cost, Demand& demand, Profit& profit, const int& numberOfIterations);
    
    virtual float getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)=0;
    
};


#endif
