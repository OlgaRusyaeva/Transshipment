//
//  Static_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 09/03/15.
//
//

#ifndef __TransshipmentProject__Static_Solver__
#define __TransshipmentProject__Static_Solver__

#include <boost/shared_ptr.hpp>

class State;
class Cost;
class PiecewiseLinearFunctionSet;
class Actions;
class iProblem;

class Static_Solver
{
public:
    Static_Solver(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const int& iterationLearning){initializePiecewiseAppVF(NoTransshipmentFunction);iterations=iterationLearning;transshipPeriod=-1;}
    ~Static_Solver();
    
    int getTransshipPeriod(){return transshipPeriod;}
    
    void solve(iProblem& problem);
    
private:
    boost::shared_ptr<PiecewiseLinearFunctionSet> F;
    int transshipPeriod;
    int iterations;
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& piecewiseVF);

    
};

#endif /* defined(__TransshipmentProject__Static_Solver__) */
