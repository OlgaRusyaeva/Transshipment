//
//  ADPLifeTimeTransshipment_Simulator.hpp
//  TransshipmentProject
//
//  Created by Olga on 22/06/16.
//
//

#ifndef ADPLifeTimeTransshipment_Simulator_hpp
#define ADPLifeTimeTransshipment_Simulator_hpp

#include "types.h"
#include "Simulator.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Actions;
class Demand;
class PLValueFunction;
class iProblem;

class ADPLifeTimeTransshipment_Simulator: public Simulator
{
public:
    ADPLifeTimeTransshipment_Simulator(const int& numberOfIterations, const int& seedNumber,const boost::shared_ptr<PLValueFunction>& piecewiseVF):Simulator(numberOfIterations,seedNumber){initializePiecewiseAppVF(piecewiseVF);}
    ~ADPLifeTimeTransshipment_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
    
    
private:
    
    boost::shared_ptr<PLValueFunction> PVF;
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PLValueFunction>& piecewiseVF);
    
    
};

#endif /* ADPLifeTimeTransshipment_Simulator_hpp */
