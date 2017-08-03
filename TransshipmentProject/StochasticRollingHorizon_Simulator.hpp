//
//  StochasticRollingHorizon_Simulator.hpp
//  TransshipmentProject
//
//  Created by Olga on 16/12/15.
//
//

#ifndef StochasticRollingHorizon_Simulator_hpp
#define StochasticRollingHorizon_Simulator_hpp

#include "SimulatorReplenishmentAndTransshipments.h"
#include "types.h"
#include <boost/shared_ptr.hpp>
#include "gurobi_c++.h"


class State;
class Actions;
class iDistribution;
class iProblem;

class StochasticRollingHorizon_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    StochasticRollingHorizon_Simulator(const int& numberOfIterations, const int& seedNumber,const int& horizon, const int& size, const boost::shared_ptr<State>& initialState):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber){horizonLength=horizon;initializeInitialState(initialState);sampleSize=size;}
    ~StochasticRollingHorizon_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix);
    
    State getInitialState(iProblem& problem,const int& iteration);
    
private:
    int horizonLength;
    boost::shared_ptr<State> IL;
    int sampleSize;
    
    void initializeInitialState(const boost::shared_ptr<State>& state);
    
    
};

#endif /* StochasticRollingHorizon_Simulator_hpp */
