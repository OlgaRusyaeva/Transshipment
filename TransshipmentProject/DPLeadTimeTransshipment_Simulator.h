//
//  DPLeadTimeTransshipment_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#ifndef __TransshipmentProject__DPLeadTimeTransshipment_Simulator__
#define __TransshipmentProject__DPLeadTimeTransshipment_Simulator__

#include "SimulatorReplenishmentAndTransshipments.h"
#include "types.h"
#include <boost/shared_ptr.hpp>


class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class Hashing;
class ValueFunction;
class iProblem;

class DPLeadTimeTransshipment_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    DPLeadTimeTransshipment_Simulator(const int& numberOfIterations, const int& seedNumber,const ValueFunction& valueFunction,const Hashing& hashFunction, const boost::shared_ptr<State>& initialState):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber){initializeValueFunction(valueFunction);initializeHashFunction(hashFunction);initializeInitialState(initialState);}
    ~DPLeadTimeTransshipment_Simulator();
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix);
    
    State getInitialState(iProblem& problem,const int& iteration);
    
private:
    ValueFunction *VF;
    Hashing *HF;
    boost::shared_ptr<State> IL;
    
    void initializeInitialState(const boost::shared_ptr<State>& state);
    
    void initializeValueFunction(const ValueFunction& valueFunction);
    
    void initializeHashFunction(const Hashing& hashFunction);
    
    
    
};

#endif /* defined(__TransshipmentProject__DPLeadTimeTransshipment_Simulator__) */
