//
//  DeterministicLeadTimeTransshipment_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#ifndef __TransshipmentProject__DeterministicLeadTimeTransshipment_Solver__
#define __TransshipmentProject__DeterministicLeadTimeTransshipment_Solver__

#include "ActionsMap.h"
#include "gurobi_c++.h"
#include <boost/shared_ptr.hpp>

class iProblem;
class iDistribution;
class State;
class Cost;
class Demand;
class LeadTimes;

typedef boost::shared_ptr<ActionsMap> actionMap_ptr;
typedef boost::shared_ptr<State> order_ptr;

class DeterministicLeadTimeTransshipment_Solver {
    
public:
    DeterministicLeadTimeTransshipment_Solver(const int& seedNumber){seed=seedNumber;}
    ~DeterministicLeadTimeTransshipment_Solver(){}
    
    void solve(iProblem& problem, const int& numberIterations);
    
    std::vector<actionMap_ptr>& getTransshipments();
    
    std::vector<order_ptr>& getInitialState();
    
private:
    int seed;
    std::vector<actionMap_ptr> transshipments;
    std::vector<order_ptr> order;
    
    void initialize(const int& location, const int& time,const int& numberIterations);
    
    void getDeterministicSolution(const int& location, const int& time, State& initialState,const  Cost& cost,Demand& demand, const int& numberIterations, const LeadTimes& leadTimes);
    
    void getDeterministicSolution(const int& location, const int& time, const  Cost& cost,Demand& demand, const int& numberIterations, const LeadTimes& leadTimes);
};

#endif /* defined(__TransshipmentProject__DeterministicLeadTimeTransshipment_Solver__) */
