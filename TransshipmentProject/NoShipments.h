//
//  NoShipments.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 1/28/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_NoShipments_h
#define TransshipmentProject_NoShipments_h

#include "CommonMethods.h"
#include "iProblem.h"

row_t solveByNoShipmentsMethod(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters);//don't use distanceMatrix

#endif
