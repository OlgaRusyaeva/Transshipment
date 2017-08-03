//
//  NoTransshipmentApp_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef TransshipmentProject_NoTransshipmentApp_Solver_h
#define TransshipmentProject_NoTransshipmentApp_Solver_h

#include "NoTransshipment_Solver.h"

class State;
class Cost;
class Demand;

class NoTransshipmentApp_Solver: public NoTransshipment_Solver
{
public:
    NoTransshipmentApp_Solver(const int& iterationNumber){N=iterationNumber;}
    
    void constructValueFunctionForRestPeriods(const int& time, const int& location, const Cost& cost, const State& currentState, Demand& demand,const matrixInt_t& identical);
private:
    int N;
};
#endif
