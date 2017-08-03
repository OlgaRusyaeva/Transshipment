//
//  ADPLeadTimeTransshipment_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#ifndef __TransshipmentProject__ADPLeadTimeTransshipment_Solver__
#define __TransshipmentProject__ADPLeadTimeTransshipment_Solver__

#include"types.h"
#include <boost/shared_ptr.hpp>
#include "array.h"

class iDistribution;
class ADP_Simulator;
class State;
class Cost;
class Actions;
class Demand;
class PLValueFunction;
class iProblem;
class LeadTimes;

class ADPLeadTimeTransshipment_Solver
{
public:
    ADPLeadTimeTransshipment_Solver(const boost::shared_ptr<State>& upperBound){epsilon=0.3;step=50;initializeState(upperBound);}
    ~ADPLeadTimeTransshipment_Solver(){}
    
    void solveAsSinglePeriod(iProblem& problem, const int& iterationNumber);
    
    void solveAsMultiPeriod(iProblem& problem, const int& iterationNumber);
    
    boost::shared_ptr<PLValueFunction> getPiecewiseApp_ValueFunction() const;
    
    boost::shared_ptr<State> getInitialState() const;
    
private:
    
    boost::shared_ptr<PLValueFunction> approxVF;
    boost::shared_ptr<State> IL;
    
    void initializePiecewiseApp_ValueFunction(const int& location, const int& time, const int& maxLeadTime);
    
    float alpha;
    int step;
    float epsilon;
    
    void initializeState(const boost::shared_ptr<State>& upperBound);
    
    void initializeLastPeriodApproximation(const int& t, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    
    void getApproximationSinglePeriodUpdate(const int& location, const int& time, const  Cost& cost,Demand& demand, const int& iterationNumber,const LeadTimes& leadTimes);
    
    void getApproximationMultiPeriodUpdate(const int& location, const int& time, const  Cost& cost,Demand& demand, const int& iterationNumber,const LeadTimes& leadTimes);
    
    
    void getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    
    void getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice);
    
    float getProfit(const int& IL, const int& d, const float& h, const float& p);
    
    float getMinGradient( arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit);
    
    float getMaxGradient( arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit);
    
    void printPLValueFunction();
    
    void setInitialState(const Cost& cost);
    
    Actions getNewDecision(const State& state, const int& rseed);
    
    
};


#endif /* defined(__TransshipmentProject__ADPLeadTimeTransshipment_Solver__) */
