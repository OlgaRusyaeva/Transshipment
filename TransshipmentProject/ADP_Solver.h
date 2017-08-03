//
//  ADP_Solver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ADP_Solver_h
#define TransshipmentProject_ADP_Solver_h

#include"types.h"
#include <boost/shared_ptr.hpp>
#include "array.h"

class iDistribution;
class ADP_Simulator;
class State;
class Cost;
//class Actions;
class Demand;
class PLValueFunction;
class iProblem;
class LeadTimes;

class ADP_Solver
{
public:
    ADP_Solver(){epsilon=0.7;step=1;}
    ~ADP_Solver(){}
    
    void solve(iProblem& problem, const int& iterationNumber);
    
    boost::shared_ptr<PLValueFunction>& getPiecewiseApp_ValueFunction();
    
    
protected:
    
    boost::shared_ptr<PLValueFunction> approxVF;
    
    void initializePiecewiseApp_ValueFunction(const int& location, const int& time, const int& lifeTime);
    
    float alpha;
    int step;
    float epsilon;
    
    void initializeLastPeriodApproximation(const int& t, const State& state, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    
    virtual void getApproximation(const int& location, const int& time, const int& lifeTime, State& initialState,const  Cost& cost,Demand& demand, const LeadTimes& leadTimes, const int& iterationNumber)=0;
    
    
    void getProfitOnePlus(row_t& profit,const State& state, const int& lifeTime, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    
    void getProfitOneMinus(row_t& profit,const State& state, const int& lifeTime, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    
    float getProfit(const int& IL, const int& d, const float& h, const float& p);
    
    void printPLValueFunction();
    
    
};



#endif
