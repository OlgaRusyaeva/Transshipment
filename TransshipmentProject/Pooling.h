//
//  Pooling.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 4/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Pooling_h
#define TransshipmentProject_Pooling_h

#include "CommonMethods.h"
#include "iProblem.h"
#include "OutputGnuplot.h"

row_t solveByPooling(const int& location, const int& time, const long& numberOfIterations,State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters);

static bool MyDataSortPredicateAscending(const pair<int,int>& lhs, const pair<int,int>& rhs);
static bool MyDataSortPredicateDescending(const pair<int,int>& lhs, const pair<int,int>& rhs);

#endif
