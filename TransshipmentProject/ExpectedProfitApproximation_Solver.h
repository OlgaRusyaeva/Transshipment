//
//  ExpectedProfitApproximation_Solver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 9/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ExpectedProfitApproximation_Solver_h
#define TransshipmentProject_ExpectedProfitApproximation_Solver_h

#include"types.h"
#include "PiecewiseApp_ValueFunction.h"

class iDistribution;
class State;
class Cost;
class Demand;
class iProblem;

class ExpectedProfitApproximation_Solver
{
public:
    ExpectedProfitApproximation_Solver(){epsilon=0.7;step=1;}
    ~ExpectedProfitApproximation_Solver(){delete approxVF;}
    
    void solve(iProblem& problem, const int& iterationNumber);
    
    PiecewiseApp_ValueFunction& getPiecewiseApp_ValueFunction() const;
    
    
private:
    
    PiecewiseApp_ValueFunction *approxVF;
    
    void initializePiecewiseApp_ValueFunction(const int& location, const int& time);
    
    float alpha;
    int step;
    float epsilon;
    
    void update(const int& t, const int& loc,  const float& new_slopePlus, const float& new_slopeMinus, const int& pos_A, const int& pos_B, const int& currentState);
    
    void insert(const int& t, const int& loc, const int& new_breakpoint, int& increase_index, int& pos);
    
    
    //should change k, u, v
    void updateSlope(const int& t, const int& loc, const float& gradientPlus, const float& gradientMinus, const int& inventory, const int& L, const int& iteration);
    
    
    void getApproximation(const int& location, const int& time, State& initialState,const  Cost& cost,Demand& demand, const int& iterationNumber);
    
    
    void getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    
    void getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    
    float getProfit(const int& IL, const int& d, const float& h, const float& p);
    
    
};


#endif
