//
//  Header ExactDP.cpp
//  


#ifndef project3_ExactDP_h
#define project3_ExactDP_h

#include "CommonMethods.h"
#include "iProblem.h"
#include <ctime>

/*Exact dynamic programming method */
void solveExactDP(const int& location, const int& time,  State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,ValueFunction& valueFunction,Hashing& hashFunction);

float getTransshipmentCostByExactDP(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,const Demand& demand, const int& currentPeriod, ValueFunction& valueFunction,Actions& maxTransshipmentMatrix,Hashing& hashFunction);

/*Evaluation procedure for the DP algorithm*/
//row_t evaluationPolicy(const ValueFunction& valueFunction, Hashing& hashFunction,const int& location, const int& time, const long& iterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters); 

/*test - change generateActionsZeroOne from L-1 to 0 for value function*/
//row_t evaluationModifiedPolicy(const ValueFunction& valueFunction, Hashing& hashFunction,const int& location, const int& time, const long& iterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters); 


#endif
