//
//  ServiceLevelMethod.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 10/30/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ServiceLevelMethod_h
#define TransshipmentProject_ServiceLevelMethod_h

#include "CommonMethods.h"
#include "iProblem.h"
#include "NormalDistribution.h"
#include "MinMax.h"

row_t solveByServiceLevelMethodReactive(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters);
row_t solveByServiceLevelMethodProactiveUsingReactiveMethods(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters);
row_t solveByServiceLevelMethodProactive(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters);

#endif
