//
//  Deterministic_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 7/31/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_Deterministic_Simulator_h
#define TransshipmentProject_Deterministic_Simulator_h

#include "Simulator.h"
#include "types.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class iDistribution;
class ActionsMap;
class iProblem;

typedef boost::shared_ptr<ActionsMap> actionMap_ptr;

class Deterministic_Simulator: public Simulator
{
public:
	Deterministic_Simulator(const int& numberOfIterations, const int& seedNumber,const std::vector<actionMap_ptr>& a_map):Simulator(numberOfIterations,seedNumber){initializeActionsMap(a_map);}
    ~Deterministic_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
private:

    std::vector<actionMap_ptr> AM;
    
    void initializeActionsMap(const std::vector<actionMap_ptr>& a_map);
    
    
    
};


#endif
