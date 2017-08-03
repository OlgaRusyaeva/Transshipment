//
//  UpperBound_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 10/03/15.
//
//

#include "UpperBound_Solver.h"
#include "PiecewiseLinearFunctionSet.h"
#include "Static_Simulator.h"
#include "iProblem.h"
#include "Profit.h"
#include "iProblem.h"

void UpperBound_Solver::initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction)
{
    F=NoTransshipmentFunction;
}

void UpperBound_Solver::initializeTransshipPeriods(const int& iterationNumber)
{
    transshipPeriods=vector_t(iterationNumber);
    for (int i=0; i<iterationNumber; i++) {
        transshipPeriods[i]=-1;
    }
}

UpperBound_Solver::~UpperBound_Solver()
{
    //delete F;
}

void UpperBound_Solver::solve(iProblem& problem)
{
    int time = problem.getTimeHorizon();
    
    matrix_t profits(time);
    
    for (int t=0; t<time; t++) {
        Static_Simulator sm(iterations,seed,F,t);
        Profit sp(problem, iterations);
        sm.simulate(problem,sp);
        profits[t]=sp.getProfit();

    }
    
    for (int i=0; i<iterations; i++) {
        float minProfit=-std::numeric_limits<float>::max();
        for (int t=0; t<time; t++) {
            if (profits[t][i]>minProfit) {
                transshipPeriods[i]=t;
                minProfit=profits[t][i];
            }
        }
    }
    
    
}

vector_t UpperBound_Solver::getTransshipPeriod()const
{
    return transshipPeriods;
}