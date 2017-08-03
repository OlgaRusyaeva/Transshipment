//
//  DeterministicRollingHorizon_Simulator.hpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

#ifndef DeterministicRollingHorizon_Simulator_hpp
#define DeterministicRollingHorizon_Simulator_hpp

#include "SimulatorReplenishmentAndTransshipments.h"
#include "types.h"
#include <boost/shared_ptr.hpp>
#include "gurobi_c++.h"


class State;
class Actions;
class iDistribution;
class iProblem;

class DeterministicRollingHorizon_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    DeterministicRollingHorizon_Simulator(const int& numberOfIterations, const int& seedNumber,const int& horizon, const boost::shared_ptr<State>& initialState):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber){horizonLength=horizon;initializeInitialState(initialState);}
    ~DeterministicRollingHorizon_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix);
    
    State getInitialState(iProblem& problem,const int& iteration);
    
private:
    int horizonLength;
    boost::shared_ptr<State> IL;
    
    void initializeInitialState(const boost::shared_ptr<State>& state);
    
    
};

#endif /* DeterministicRollingHorizon_Simulator_hpp */
