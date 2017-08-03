//
//  ProactiveMethods.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ProactiveMethods_h
#define TransshipmentProject_ProactiveMethods_h

#include
#include "iProblem.h"

float gettransshipmentCostByTBA(const int& location, const int& time,  State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, const int& currentPeriod,Actions& maxTransshipmentMatrix);

float gettransshipmentCostByTBA2(const int& location, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, const int& currentPeriod, const vector_t& maxDemandForDistribution,Actions& maxTransshipmentMatrix);

float gettransshipmentCostByTIE(const int& location, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution,const int& numberOfTransshipments,Actions& maxTransshipmentMatrix);

vector_t calculateLowerBound(vector<boost::shared_ptr<iDistribution> > distribution,const int& numberOfLocations);
vector_t calculateUpperBound(vector<boost::shared_ptr<iDistribution> > distribution,const int& time, const int& numberOfLocations);
vector_t getBound(const State& state,vector<boost::shared_ptr<iDistribution> > distribution,const int& numberOfLocations);


#endif
