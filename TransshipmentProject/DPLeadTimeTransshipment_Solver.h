//
//  DPLeadTimeTransshipment_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#ifndef __TransshipmentProject__DPLeadTimeTransshipment_Solver__
#define __TransshipmentProject__DPLeadTimeTransshipment_Solver__

#include "types.h"
#include "ValueFunction.h"
#include "Hashing.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Demand;
class iProblem;
class iDistribution;
class LeadTimes;

class DPLeadTimeTransshipment_Solver
{
public:
    DPLeadTimeTransshipment_Solver(const boost::shared_ptr<State>& upperBound){initializeValueFunction();initializeHashFunction();initializeState(upperBound);}
    ~DPLeadTimeTransshipment_Solver();
    
    void solve(iProblem& problem,const std::string& f_fileNameVF, const std::string& f_fileNameI);
    
    void solve(const std::string& f_fileNameVF, const std::string& f_fileNameI);
    
    ValueFunction& getValueFunction() const;
    
    Hashing& getHashFunction() const;
    
    boost::shared_ptr<State> getInitialState() const;
    
protected:
    
    ValueFunction *valueFunction;
    Hashing *hashFunction;
    boost::shared_ptr<State> initialState;
    
    void initializeValueFunction();
    
    void initializeHashFunction();
    
    void initializeState(const boost::shared_ptr<State>& upperBound);
    
    void outputLookUpTable(const std::string& f_fileNameVF);
    
    void outputInitialState(const std::string& f_fileNameIL);
    
    void setLookUpTable(const int& location, const int& time, const Cost& cost, Demand& demand, const LeadTimes& leadtimes);
    
    int getUpperBoundForIL();
    
    float getExpectedProfit(const int& t, const int& time, const State& state, Demand& demand, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const LeadTimes& leadtimes) const;
    
    float getLastPeriodValueFunction(const State& state, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    bool stateDoesNotHavePipeline(const State& currentState);
    
    bool proceedState(const State& state, const LeadTimes& leadTimes, const int& maxIL);
    
    
};


#endif /* defined(__TransshipmentProject__DPLeadTimeTransshipment_Solver__) */
