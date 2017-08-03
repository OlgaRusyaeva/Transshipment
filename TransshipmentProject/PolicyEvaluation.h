//
//  PolicyEvaluation.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 4/29/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_PolicyEvaluation_h
#define TransshipmentProject_PolicyEvaluation_h

#include "CommonMethods.h"
#include "iProblem.h"

#include "ReactiveMethods.h"
#include "ProactiveMethods.h"
#include "ADP.h"
#include "ExactDP.h"

row_t policyEvaluation(const int& location, const int& time, const long& numberOfIterations,State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters, ValueFunction& valueFunction, Hashing& hashFunction, matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope, matrixInt_t& matrixOfClusters, const int& typeOfMethod);

//float getTransshipmentCostByProactiveMethods(const int& location, const int& time,  State& state, const Cost& cost, const Actions& distanceMatrix, vector<boost::shared_ptr<iDistribution> > distribution, const int& currentPeriod,const int& typeOfMethod,const vector_t& maxDemand,const int& numberOfTransshipments,Actions& maxTransshipmentMatrix);



#endif
