//
//  ADPLeadTimeTransshipmentM_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#ifndef __TransshipmentProject__ADPLeadTimeTransshipmentM_Simulator__
#define __TransshipmentProject__ADPLeadTimeTransshipmentM_Simulator__

#include "types.h"
#include "SimulatorReplenishmentAndTransshipments.h"
#include <boost/shared_ptr.hpp>


class State;
class Cost;
class Actions;
class Demand;
class PLValueFunction;
class iProblem;

class ADPLeadTimeTransshipmentM_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    ADPLeadTimeTransshipmentM_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PLValueFunction>& piecewiseVF, const boost::shared_ptr<State>& initialState):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber){initializePiecewiseAppVF(piecewiseVF);initializeInitialState(initialState);}
    ~ADPLeadTimeTransshipmentM_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix);
    
     State getInitialState(iProblem& problem,const int& iteration);
    
private:
    
    boost::shared_ptr<PLValueFunction> PVF;
    boost::shared_ptr<State> IL;
    
    void initializeInitialState(const boost::shared_ptr<State>& state);
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PLValueFunction>& piecewiseVF);
    
    
};

#endif /* defined(__TransshipmentProject__ADPLeadTimeTransshipmentM_Simulator__) */
