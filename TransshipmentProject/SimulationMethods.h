//
//  SimulationMethods.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_SimulationMethods_h
#define TransshipmentProject_SimulationMethods_h

#include "types.h"
#include "array.h"

class State;

namespace simulationMethods{
    //generate all possible demands for the state
    void generatePossibleDemands(const State& state, arrays::Array2D<float>& S);
    void getIndexVector(State& vectorOL, const vecMatrixInt& actionsForState,int& size);
    
    State generateInventoryLevel(vector_t::iterator itBegin,int& inv,const int& location, const int& leadTime);
    
    State generateInventoryLevel(const int& maxValue,int& inv,const int& location, const int& leadTime);
    
    int getRandomNumber(const int& min, const int& max);
};

#endif
