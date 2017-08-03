//
//  Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Simulator_h
#define TransshipmentProject_Simulator_h

#include"types.h"

class iProblem;
class Profit;
class State;
class Actions;
class Demand;
class Cost;
class iDistribution;

class Simulator
{
public:
    
    Simulator(const int& numberOfIterations, const int& seedNumber){seed=seedNumber;iterations=numberOfIterations;}
    
    virtual ~Simulator(){}
    
    void simulate(iProblem& problem, Profit& profit);
    
    virtual float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)=0;
    
    
protected:
    int seed;
    int iterations;
    //get all Locations that out of stock after demand satisfaction
    vector_t getStockOutLocations(const State& initialState);

    //initialize transshipment matrix by zero
    matrixInt_t initializeTransshipmentMatrix(const int& size_row, const int& size_col);
    
    void printMatrix(const int& limit1,const int& limit2, const matrixInt_t& M);
    
};




#endif
