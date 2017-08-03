//
//  ADP_aggregSimulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ADP_aggregSimulator_h
#define TransshipmentProject_ADP_aggregSimulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class Hashing;
class ValueFunction;

class ADP_aggregSimulator: public Simulator
{
public:
    ADP_aggregSimulator(int N): Simulator(N){initializeValueFunction();initializeHashFunction();}
    ~ADP_aggregSimulator(){}
    
    //void simulator(const int& location, const int& time, const State& initialState,const Cost& cost, const Actions& distanceMatrix, Demand& demand, row_t& solution);

     float getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost, const Actions& distanceMatrix,std::vector<boost::shared_ptr<iDistribution> > distribution, Actions& maxTransshipmentMatrix);
    
    
    ValueFunction& getValueFunction() const;
    
    Hashing& getHashFunction() const;
    
    
private:
    
    ValueFunction *VF;
    Hashing *HF;
    
    void initializeValueFunction();
    
    void initializeHashFunction();
    
    
    //float getTransshipmentCostByADP(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,const Demand& demand, const int& currentPeriod, std::vector<boost::shared_ptr<iDistribution> > distribution, Actions& maxTransshipmentMatrix);
    
    
};


#endif
