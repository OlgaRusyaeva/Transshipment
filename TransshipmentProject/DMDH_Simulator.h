//
//  DMDH_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef __TransshipmentProject__DMDH_Simulator__
#define __TransshipmentProject__DMDH_Simulator__

#include "types.h"
#include "SimulatorOneTransshipment.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PiecewiseLinearFunctionSet;
class iProblem;

class DMDH_Simulator: public SimulatorOneTransshipment
{
public:
    DMDH_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const int& replicationNumber):SimulatorOneTransshipment(numberOfIterations,seedNumber,NoTransshipmentFunction){rn=replicationNumber;setThreadNumber(replicationNumber);}
    
    bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration);
    
private:
    int rn;
    float dampingFactor=1;
    int threadNumber;
    
    float getExpectedProfitNextPeriodWithTransshipment(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    void getExpectedProfitNextPeriodForFixedDemand(const int& location, const Cost& cost, Demand& demand,const int& currentPeriod, const State& currentState,const std::vector<boost::shared_ptr<iDistribution> >& distribution, float& expectedProfit);
    
    void setThreadNumber(const int& replicationNumber);
    
    
};

#endif /* defined(__TransshipmentProject__DMDH_Simulator__) */
