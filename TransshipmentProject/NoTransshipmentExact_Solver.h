//
//  NoTransshipmentExact_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef TransshipmentProject_NoTransshipmentExact_Solver_h
#define TransshipmentProject_NoTransshipmentExact_Solver_h

#include "NoTransshipment_Solver.h"

class State;
class Cost;
class Demand;

class NoTransshipmentExact_Solver: public NoTransshipment_Solver
{

public:
    
    void constructValueFunctionForRestPeriods(const int& time, const int& location, const Cost& cost, const State& currentState, Demand& demand,const matrixInt_t& identical);
};
#endif
