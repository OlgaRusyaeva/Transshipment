//
//  NoTransshipment_Solver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 9/22/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_NoTransshipment_Solver_h
#define TransshipmentProject_NoTransshipment_Solver_h

#include"types.h"
#include "PiecewiseLinearFunctionSet.h"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

class iDistribution;
class State;
class Cost;
class Demand;
class iProblem;

class NoTransshipment_Solver
{
public:
    NoTransshipment_Solver(){}
    ~NoTransshipment_Solver(){}
    
    void solve(iProblem& problem);
    
    boost::shared_ptr<PiecewiseLinearFunctionSet> getExpectedProfitWithoutTransshipments() const;
    
    boost::shared_ptr<State> getUpperBound()const;
    
    void constructValueFunctionForRestPeriods(const int& time, const int& location, const Cost& cost, const State& currentState, Demand& demand,const matrixInt_t& identical);
    
    
protected:
    
    boost::shared_ptr<PiecewiseLinearFunctionSet> PVF;
    boost::shared_ptr<State> UB;
    
    void initialize(const int& location, const int& time);
    
    
    void initializeLastPeriodValueFunction(const int& T, const int L, const vector_t& upperBound, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution,const matrixInt_t& identical);

    
    //void constructAppValueFunctionForRestPeriods(const int& transshipPeriod, const int& time, const int& location, const Cost& cost, const State& currentState, Demand& demand, const int& iterationNumber);
    
    void getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    
    void getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    void setUpperBound(const Cost& cost);
    
    
};


#endif
