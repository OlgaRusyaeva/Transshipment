//
//  OPOS_Simulator.h
//  TransshipmentProject
//
//  Created by Olga on 11/12/14.
//
// One Period Optimal Solution - OPOS

#ifndef TransshipmentProject_OPOS_Simulator_h
#define TransshipmentProject_OPOS_Simulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Actions;
class iDistribution;
class iProblem;

class OPOS_Simulator: public Simulator
{
public:
    OPOS_Simulator(const int& numberOfIterations, const int& seedNumber):Simulator(numberOfIterations,seedNumber){}
    ~OPOS_Simulator(){}
    
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
    
    
    
private:
    
    /*--------------------------------help methods------------------------------------------------*/
    
    int countConstraintSize(const vector_t& source);
    
    void findTransshipments(iProblem& problem, const State& state, const vector_t& source, const vector_t& receive, const std::vector<boost::shared_ptr<iDistribution> >& distribution,Actions& maxTransshipmentMatrix);
    
    int findTransshipmentAmount(iProblem& problem, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& location1, const int& location2, const int& inv1, const int& inv2);
    
    float derivative(iProblem& problem, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& location1, const int& location2, const double& inv1, const double& inv2);
    
    void value_function(double *f, double *x, iProblem& problem, const std::vector<boost::shared_ptr<iDistribution> >& distribution,const vector_t& source, const vector_t& receive, const State& state);
    
    void constraints(double *g, double *x, const vector_t& source, const vector_t& receive, const State& state);
    
    
};


#endif
