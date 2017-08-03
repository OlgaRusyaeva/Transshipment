//
//  ADPOnline_Simulator.hpp
//  TransshipmentProject
//
//  Created by Olga on 21/03/16.
//
//

#ifndef ADPOnline_Simulator_hpp
#define ADPOnline_Simulator_hpp

#include "types.h"
#include "SimulatorReplenishmentAndTransshipments.h"
#include <boost/shared_ptr.hpp>
#include "array.h"
#include "randomNumber.h"

class State;
class Cost;
class Actions;
class Demand;
class PLValueFunction;
class iProblem;
class LeadTimes;

class ADPOnline_Simulator: public SimulatorReplenishmentAndTransshipments
{
public:
    ADPOnline_Simulator(const int& numberOfIterations, const int& seedNumber,iProblem& problem,const boost::shared_ptr<State>& upperBound, const int& numberOfSamplePaths);
    ~ADPOnline_Simulator(){}
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix);
    
    State getInitialState(iProblem& problem, const int& iteration);
    
private:
    
    float alpha;
    int step;
    float epsilon;
    int sampleNumber;
    
    boost::shared_ptr<PLValueFunction> approxVF;
    boost::shared_ptr<State> IL;
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    std::vector<boost::shared_ptr<randomIntNumber> > randomILGenerator;
    
    void initializeValueFunction(iProblem& problem);
    
    void initializeState(const boost::shared_ptr<State>& upperBound);
    
    void initializeLastPeriodApproximation(const int& t, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    void updateApproximation(const int& location, const int& time,const  Cost& cost,State& currentState, Demand& demand, const LeadTimes& leadTimes, const int& iterationNumber, const int& currentPeriod);
    
    void getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    void getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    float getMinGradient(arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit);
    
    float getMaxGradient(arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit);
    
    void printPLValueFunction();
    
    State getSampleState(const State& currentState, const Cost& cost, const int& currentPeriod,const LeadTimes& leadTimes);
    
    State getOrderDecision(const int& location, const int& maxLeadTime, const Cost& cost);
    
    State getSampleInitialState(const int& location, const int& maxLeadTime);
    
    
};


#endif /* ADPOnline_Simulator_hpp */
