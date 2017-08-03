//
//  ADP.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ADP_h
#define TransshipmentProject_ADP_h
#include "CommonMethods.h"
#include "iProblem.h"
#include "OutputGnuplot.h"

void solveByADP(const int& location, const int& time, const long& numberOfIterations,State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,ValueFunction& valueFunction,Hashing& hashFunction, matrixInt_t& matrixOfClusters);

float getTransshipmentCostByADP(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,const Demand& demand, const int& currentPeriod, vector<boost::shared_ptr<iDistribution> > distribution, ValueFunction& valueFunction,Actions& maxTransshipmentMatrix,Hashing& hashFunction,const matrixInt_t& matrixOfClusters);

//row_t evaluationADPPolicy(const ValueFunction& valueFunction, Hashing& hashFunction,const int& location, const int& time, const long& iterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,const matrixInt_t& matrixOfClusters); 

void updateSlope(int& k, row_t& u, row_t& v, const float& gradient, const int& inventory, const int& L,const int& iteration);

void insert(int& k, row_t& u, row_t& v,const int& new_breakpoint, int& increase_index, int& position);

void update(row_t& v, const float& new_slope, const int& pos_A, const int& pos_B, const float& alpha);

void solveByADP_piecewise(const int& location, const int& time, const long& numberOfIterations, State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope);

float getTransshipmentCostByADP_piecewise(matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope,const int& currentPeriod, State& state,const Cost& cost, const Actions& distanceMatrix, Actions& maxTransshipmentMatrix);

/*row_t evaluationADP_piecewisePolicy(matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope,const int& location, const int& time, const long& numberOfIterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters);*/





#endif
