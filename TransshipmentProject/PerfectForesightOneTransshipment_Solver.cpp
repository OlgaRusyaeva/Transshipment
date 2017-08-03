//
//  PerfectForesightOneTransshipment_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 06/03/15.
//
//

#include "PerfectForesightOneTransshipment_Solver.h"
#include "iProblem.h"
#include "PiecewiseApp_ValueFunction.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "LP_optimizer.h"
#include "Actions.h"

PerfectForesightOneTransshipment_Solver::~PerfectForesightOneTransshipment_Solver()
{
    delete F;
}

void PerfectForesightOneTransshipment_Solver::initializePiecewiseAppVF(const PiecewiseApp_ValueFunction& NoTransshipmentFunction)
{
    F=new PiecewiseApp_ValueFunction(NoTransshipmentFunction.getPrameterA(),NoTransshipmentFunction.getPrameterB());
    *F=NoTransshipmentFunction;
}

void PerfectForesightOneTransshipment_Solver::initializeTransshipmentPeriodVector(const int& numberOfIterations)
{
    transshipmentPeriods=vector_t(numberOfIterations);
    
    for (int i=0; i<numberOfIterations; i++) {
        transshipmentPeriods[i]=-1;
    }
}

vector_t& PerfectForesightOneTransshipment_Solver::getTransshipmentPeriods()
{
    return transshipmentPeriods;
}

void PerfectForesightOneTransshipment_Solver::solve(iProblem& problem)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    
    State initialState = problem.getState();
    Cost costInState = problem.getCost();
    Demand demand=problem.getDemand();
    
    getVectorOfTransshipmentPeriods(L,T,initialState, costInState, demand);


}

void PerfectForesightOneTransshipment_Solver::getVectorOfTransshipmentPeriods(const int& location, const int& time, const State& initialState,const Cost& cost,Demand& demand)
{

}


//if I know all my demand in advance, the decision probably will be different
//TODO change this function
float PerfectForesightOneTransshipment_Solver::getTransshipmentCost(const Cost& cost,const int& currentPeriod, const State& currentState, Actions& maxTransshipmentMatrix)const
{
    
    LP_optimizer lp_Problem;
    //maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, PVF->getIndexVector(0), PVF->getBreakpoints(0), PVF->getSlope(0));
    maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, F->getIndexVector(currentPeriod), F->getBreakpoints(currentPeriod), F->getSlope(currentPeriod));
    
    return lp_Problem.optimalObjectiveFunction();
    
}
