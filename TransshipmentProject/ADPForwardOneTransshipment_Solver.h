//
//  ADPForwardOneTransshipment_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 04/03/15.
//
//

#ifndef TransshipmentProject_ADPForwardOneTransshipment_Solver_h
#define TransshipmentProject_ADPForwardOneTransshipment_Solver_h

#include "ADPForward_Solver.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Demand;
class iProblem;
class iDistribution;
class PiecewiseLinearFunctionSet;
class Actions;

class ADPForwardOneTransshipment_Solver:public ADPForward_Solver
{
public:
    ADPForwardOneTransshipment_Solver(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction){initializePiecewiseAppVF(NoTransshipmentFunction);}
    ~ADPForwardOneTransshipment_Solver();
    
private:
    
    boost::shared_ptr<PiecewiseLinearFunctionSet> F;
    
    float getCostForActions(State& state,const int& currentPeriod,const Cost& cost, const int& time, const arrays::Array2D<float>& possibleDemands)const;
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& piecewiseVF);
    
    float getTransshipmentCost(const Cost& cost,const int& currentPeriod, const State& currentState, Actions& maxTransshipmentMatrix)const;
    
};


#endif
