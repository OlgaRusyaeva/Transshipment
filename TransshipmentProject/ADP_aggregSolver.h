//
//  ADP_aggregSolver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ADP_aggregSolver_h
#define TransshipmentProject_ADP_aggregSolver_h

#include "types.h"

class ADP_aggregSimulator;
class iDistribution;
class ADP_Simulator;
class Cost;
class Actions;
class Demand;
class Hashing;
class ValueFunction;
class iProblem;
class State;


class ADP_aggregSolver
{
public:
    ADP_aggregSolver(int it){epsilon=0.2;iterationNumber=it;}
    ~ADP_aggregSolver(){}
    
    void solve(iProblem& problem, ADP_aggregSimulator& policy, const std::string& f_fileName);
    
private:
    float epsilon;
    int iterationNumber;
    
    void getLookUpTable(const int& location, const int& time, const State& initialState,const Cost& cost, const Actions& distanceMatrix, Demand& demand, ValueFunction& valueFunction,Hashing& hashFunction);
    
    void getLookUpTable(ValueFunction& valueFunction,const std::string& f_fileName);
    
    
    void outputLookUpTable(ValueFunction& valueFunction,const std::string& f_fileName);
    
    void outputClusters(const int&location,const Actions& distance,const vector_t& clusters);
    
    void sortAvailableQuantity(list_t& availableQuantity,row_t& distanceToFirstShortageElement);
};


#endif
