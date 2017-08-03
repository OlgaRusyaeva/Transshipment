//
//  ADP_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ADP_Simulator_h
#define TransshipmentProject_ADP_Simulator_h

#include "types.h"
#include "Simulator.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PLValueFunction;
class iProblem;

class ADP_Simulator: public Simulator
{
public:
    ADP_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PLValueFunction>& piecewiseVF):Simulator(numberOfIterations,seedNumber){initializePiecewiseAppVF(piecewiseVF);}
    ~ADP_Simulator(){}
    
     float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
    
    
private:
    
    boost::shared_ptr<PLValueFunction> PVF;
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PLValueFunction>& piecewiseVF);
      
    
};



#endif
