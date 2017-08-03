//
//  DRA_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 07/03/15.
//
//

#include "DRA_Solver.h"
#include <math.h>
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "PiecewiseApp_ValueFunction.h"
#include "LP_optimizer.h"
#include "Actions.h"
#include "SimulationMethods.h"
#include "ProblemMethods.h"
#include <math.h>

using namespace std;

void DRA_Solver::solve(iProblem& problem)
{
    State state=problem.getState();
    Demand demand=problem.getDemand();
    int time=problem.getTimeHorizon();
    int location = problem.getStateDimension();
    
    replicationNumber=(int)pow(iterationNumber,(float)1/time);
    replicationVector=vector_t(time);
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    float averageExpectedProfit=getExpectedProfit(problem,0, state, demand, distribution);
    
    cout<<averageExpectedProfit<<'\n';
}

float DRA_Solver::getExpectedProfit(iProblem& problem,const int& currentPeriod, const State& currentState, Demand& demand, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    int time=problem.getTimeHorizon();
    
    
    if (currentPeriod==time) {
        return 0;
    }
    else
    {
        float expectedProfit=0;
        float currentProfit=getExpectedProfitWithCurrentTransshipment(cost, currentPeriod, currentState);
        
        for (int r=0; r<replicationNumber; r++) {
            
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
            }
            
            float reward = problemMethods::getReward(cost.getSalesCost(),currentState,demand);
            
            State stateAfterDemandSatisfaction=currentState;
            
            problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
            
            float holdingCost = problemMethods::getHoldingCost(cost.getHoldingCost(),stateAfterDemandSatisfaction);
            
            expectedProfit+=reward-holdingCost;
            
            expectedProfit+=getExpectedProfit(problem,currentPeriod+1,stateAfterDemandSatisfaction,demand, distribution);
            
            replicationVector[currentPeriod]++;
        }
        //CHECK
        
        int increment=0;
        if (currentPeriod!=0) {
            increment=replicationVector[currentPeriod-1]*(int)pow(replicationNumber,(float)time-currentPeriod);
        }
        if (currentProfit>(float)expectedProfit/replicationNumber) {
            for (int r=0; r<(int)pow(replicationNumber,(float)time-currentPeriod); r++) {
                if ((r+increment)>iterationNumber) {
                    cout<<"ERROR"<<'\n';
                }
                transshipPeriods[r+increment]=currentPeriod;
            }
            return currentProfit;
        }
        else
            return (float)expectedProfit/replicationNumber;
    }
    
}

vector_t DRA_Solver::getTransshipPeriods()const
{
    return transshipPeriods;
}

void DRA_Solver::initializePiecewiseAppVF(const PiecewiseApp_ValueFunction& NoTransshipmentFunction)
{
    F=new PiecewiseApp_ValueFunction(NoTransshipmentFunction.getPrameterA(),NoTransshipmentFunction.getPrameterB());
    *F=NoTransshipmentFunction;
}

void DRA_Solver::initializeTransshipPeriods(const int& iterations)
{
    transshipPeriods=vector_t(iterations);
    for (int i=0; i<iterations; i++) {
        transshipPeriods[i]=-1;
    }
}

DRA_Solver::~DRA_Solver()
{
    delete F;
}

float DRA_Solver::getExpectedProfitWithCurrentTransshipment(const Cost& cost,const int& currentPeriod, const State& currentState)const
{
    Actions maxTransshipmentMatrix(currentState.getLength());
    LP_optimizer lp_Problem;

    maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, F->getIndexVector(currentPeriod), F->getBreakpoints(currentPeriod), F->getSlope(currentPeriod));

    
    return lp_Problem.optimalObjectiveFunction();
}