//
//  LookaheadMultyPeriods_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 01/03/15.
//
//

#ifndef TransshipmentProject_LookaheadMultyPeriods_Simulator_h
#define TransshipmentProject_LookaheadMultyPeriods_Simulator_h

#include "Simulator.h"
#include "types.h"


class State;
class Cost;
class Actions;
class Demand;
class PiecewiseLinearFunctionSet;
class iProblem;

class LookaheadMultyPeriods_Simulator: public Simulator
{
public:
    LookaheadMultyPeriods_Simulator(const int& numberOfIterations, const int& seedNumber,const PiecewiseLinearFunctionSet& noTransshipmentF):Simulator(numberOfIterations,seedNumber){initializeNoTransshipmentFunction(noTransshipmentF);}
    ~LookaheadMultyPeriods_Simulator();
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);

    
private:
    
    PiecewiseLinearFunctionSet *F;
    
    void initializeNoTransshipmentFunction(const PiecewiseLinearFunctionSet& noTransshipmentF);
    
    row_t getTransshipmentProfitRestPeriods(const State& currentState, const int& currentPeriod);
};


#endif
