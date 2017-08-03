//
//  DPOneTransshipment_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef TransshipmentProject_DPOneTransshipment_Solver_h
#define TransshipmentProject_DPOneTransshipment_Solver_h

#include "types.h"
#include "DP_Solver.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Demand;
class iProblem;
class iDistribution;
class PiecewiseLinearFunctionSet;
class Actions;

class DPOneTransshipment_Solver:public DP_Solver
{
public:
    DPOneTransshipment_Solver(const  boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction){initializePiecewiseAppVF(NoTransshipmentFunction);}
    ~DPOneTransshipment_Solver(){}
    
private:
    
	 boost::shared_ptr<PiecewiseLinearFunctionSet> F;
    
    float getMaxProfit(const int& t, const int& time, const State& state, Demand& demand, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution) const;
    
    void initializePiecewiseAppVF(const  boost::shared_ptr<PiecewiseLinearFunctionSet>& piecewiseVF);
    
    float getTransshipmentCost(const Cost& cost,const int& currentPeriod, const State& currentState, Actions& maxTransshipmentMatrix)const;
    
    //float getExpectedProfitNextPeriodsWithoutTransshipments(const int& location,const int& currentPeriod, const State& currentState)const;
};


#endif
