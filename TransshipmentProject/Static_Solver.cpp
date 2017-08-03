//
//  Static_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 09/03/15.
//
//

#include "Static_Solver.h"
#include "PiecewiseLinearFunctionSet.h"
#include "Static_Simulator.h"
#include "iProblem.h"
#include "Profit.h"
#include "iProblem.h"

void Static_Solver::initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction)
{
    F=NoTransshipmentFunction;
}

Static_Solver::~Static_Solver()
{
    //delete F;
}

void Static_Solver::solve(iProblem& problem)
{
    int time = problem.getTimeHorizon();
    int seed=20;
    
    
    float sum=-std::numeric_limits<float>::max();
    
    for (int t=0; t<time; t++) {
        Static_Simulator sm(iterations,seed,F,t);
        Profit sp(problem, iterations);
        sm.simulate(problem,sp);
        
        if (sp.getAverageProfit()>=sum) {
            sum=sp.getAverageProfit();
            transshipPeriod=t;
        }
    }
    
}