//
//  Deterministic_Solver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 7/31/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Deterministic_Solver_h
#define TransshipmentProject_Deterministic_Solver_h

#include "ActionsMap.h"
#include "gurobi_c++.h"
#include <boost/shared_ptr.hpp>

class iProblem;
class iDistribution;
class State;
class Cost;
class Demand;

typedef boost::shared_ptr<ActionsMap> actionMap_ptr;

class Deterministic_Solver {
    
public:
    Deterministic_Solver(const int& seedNumber){seed=seedNumber;}
    ~Deterministic_Solver(){}
    
    void solve(iProblem& problem, const int& numberIterations);
    
    std::vector<actionMap_ptr>& getActionsMap();
    
private:
    int seed;
    std::vector<actionMap_ptr> actionsMap;
    
    void initializeActionsMap(const int& location, const int& time,const int& numberIterations);
    void getDeterministicSolution(const int& location, const int& time, State& initialState,const  Cost& cost,Demand& demand, const int& numberIterations);
};


#endif
