//
//  BasisFunctionApproximation_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#include "BasisFunctionApproximation_Solver.h"
#include "iProblem.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "BasisFunction.h"
#include "BFValueFunction.h"
#include "QP_optimizer.h"
#include "iDistribution.h"
#include "randomNumber.h"
#include "ProblemMethods.h"
#include "LP_optimizer.h"
#include "Actions.h"
#include "NumbersOperations.h"
#include "PiecewiseLinearFunctionSet.h"
#include <boost/thread.hpp>
#include <boost/thread/detail/thread_group.hpp>


BasisFunctionApproximation_Solver::~BasisFunctionApproximation_Solver()
{
    delete valueFunction;
    //delete F;
}

void BasisFunctionApproximation_Solver::solve(iProblem& problem)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();

    
    State initialState = problem.getState();
    Demand demand=problem.getDemand();
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),L,20);

    
    valueFunction->initialize(T);
    std::vector<std::vector<State> > samplePath(iterations);
    
    vector<boost::shared_ptr<BasisFunction> > bfSet(iterations);
    row_t valueToEstimate(iterations);
    
    generateSamplePath(problem,samplePath,distribution);
    
    for (int t=T-1; t>=0; t--) {


        for (int n=0; n<iterations; n++) {
            State state(L);
            
            state=samplePath[n][t];

            valueToEstimate[n]=getExpectedProfitBasedOnApproximation(problem,t,state,distribution);

            
            bfSet[n]=boost::shared_ptr<BasisFunction>(new BasisFunction(state));
            
        }

        QP_optimizer qp;
        valueFunction->set(t,qp.optimize(bfSet,bfSet[0]->getSize(),valueToEstimate));
    }
    
    
}

float BasisFunctionApproximation_Solver::getExpectedProfitBasedOnApproximation(iProblem& problem,const int& currentPeriod, const State& currentState,const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    int location = problem.getStateDimension();
    int time=problem.getTimeHorizon();
    int threadCallNumber=demandNumber/threadNumber;

    
    if (currentPeriod==time-1) {
        return getExpectedProfitNextPeriodsWithoutTransshipments(location,currentPeriod,currentState);
    }
    else
    {
        Cost cost = problem.getCost();
        Demand demand=problem.getDemand();
        
        row_t expectedProfit(demandNumber);
        //Right way: to do it for any possible demand realization
        boost::thread_group groupOfThreads;
        for (int th=0; th<threadCallNumber; th++) {
            for (int r=0; r<threadNumber; r++) {
                groupOfThreads.create_thread(boost::bind(&BasisFunctionApproximation_Solver::getExpectedProfitNextPeriodForFixedDemand,this,location,cost,demand,currentPeriod,currentState,distribution,boost::ref(expectedProfit[r+threadNumber*th])));
                
            }
            groupOfThreads.join_all();
            
        }
        
        float sum=0;
        for (int i=0; i<demandNumber; i++) {
            sum+=expectedProfit[i];
        }
        
        return sum/demandNumber;
    }
}

void BasisFunctionApproximation_Solver::getExpectedProfitNextPeriodForFixedDemand(const int& location, const Cost& cost, Demand& demand,const int& currentPeriod, const State& currentState,const std::vector<boost::shared_ptr<iDistribution> >& distribution, float& expectedProfit)
{
    
    
    for (int i=0; i<location; i++) {
        demand.setElement(i, (int)distribution[i]->sample());
    }
    
    float reward = problemMethods::getReward(cost.getSalesCost(),currentState,demand);
    
    State stateAfterDemandSatisfaction=currentState;
    
    problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
    
    float holdingCost = problemMethods::getCost(cost.getHoldingCost(),stateAfterDemandSatisfaction);
    
    float expectedProfitNextPeriodsWithCurrentTransshipment=getExpectedProfitWithCurrentTransshipment(cost, currentPeriod+1, stateAfterDemandSatisfaction);
    
    expectedProfit=reward-holdingCost+numbersOperations::getMaxValue(expectedProfitNextPeriodsWithCurrentTransshipment,calculateValueForStateBasedOnApproximation(currentPeriod+1,stateAfterDemandSatisfaction));
    
}



void BasisFunctionApproximation_Solver::generateSamplePath(iProblem& problem,std::vector<std::vector<State> >& path,const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    int location = problem.getStateDimension();
    int time=problem.getTimeHorizon();
    
    Demand demand=problem.getDemand();
    State initialState = problem.getState();
    
    for (int n=0; n<iterations; n++) {
        State currentState=initialState;
        for (int t=0; t<time; t++) {
            path[n].push_back(currentState);
            
            for (int i=0; i<location; i++) {
                demand.setElement(i,distribution[i]->sample());
            }
            
            problemMethods::getNewState(currentState, demand);
        }
    }
}

float BasisFunctionApproximation_Solver::calculateValueForStateBasedOnApproximation(const int& currentPeriod, const State& state)
{
    BasisFunction bf(state);
    
    float value=0;
    for (int i=0; i<(int)bf.getSize(); i++) {
        value+=bf.get()[i]*valueFunction->get()[currentPeriod][i];
    }
    value+=valueFunction->get()[currentPeriod][(int)bf.getSize()];
    
    return value;
}

void BasisFunctionApproximation_Solver::initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction)
{
    F=NoTransshipmentFunction;
}

//TODO: find a better approach
/*int BasisFunctionApproximation_Solver::generateBorder(const State& initialState, const int& currentPeriod,const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& currentLocation)
{
    int sum=0;
    for (int i=0; i<initialState.getLength(); i++) {
        sum+=initialState.getElement(i);
    }
    sum=(int)sum/initialState.getLength();
    
    if (initialState.getElement(currentLocation)>sum) {
        return initialState.getElement(currentLocation);
    }
    else
        return sum;
    return (int)(numbersOperations::getMaxValue(initialState.getElement(currentLocation)-0.6*distribution[currentLocation]->mean()*currentPeriod,0));

}*/

BFValueFunction& BasisFunctionApproximation_Solver::getBFValueFunction() const
{
    return *valueFunction;
}


void BasisFunctionApproximation_Solver::initializeValueFunction()
{
    valueFunction = new BFValueFunction();
}

//change to get an optimal value then we don't need getExpectedProfitNextPeriodsWithoutTransshipments
float BasisFunctionApproximation_Solver::getExpectedProfitWithCurrentTransshipment(const Cost& cost,const int& currentPeriod, const State& currentState)
{
    //int time=problem.getTimeHorizon();
    Actions maxTransshipmentMatrix(currentState.getLength());
    LP_optimizer lp_Problem;
    //maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, PVF->getIndexVector(0), PVF->getBreakpoints(0), PVF->getSlope(0));
    maxTransshipmentMatrix = lp_Problem.optimize(currentState, 0,cost, F,currentPeriod);
    
    //problemMethods::getNewState(currentState, maxTransshipmentMatrix);
    
    //float transshipmentCost = problemMethods::getTransshipmentCost(cost,maxTransshipmentMatrix);
    
    return lp_Problem.optimalObjectiveFunction();
    
}


float BasisFunctionApproximation_Solver::getExpectedProfitNextPeriodsWithoutTransshipments(const int& location,const int& currentPeriod, const State& currentState)
{
    
    float expectedProfitNextPeriodsWithoutTransshipments=0;
    for (int i=0; i<location; i++) {
        for (int j=0; j<F->get(currentPeriod,i).getBreakpointClosestTo(currentState.getElement(i,0)); j++) {
            //check
            expectedProfitNextPeriodsWithoutTransshipments+=F->get(currentPeriod,i).getSlope(F->get(currentPeriod,i).getBreakpoint(j));
        }
    }
    
    return expectedProfitNextPeriodsWithoutTransshipments;
    
}

void BasisFunctionApproximation_Solver::setThreadNumber(const int& replicationNumber)
{
    int maxThreadNumber=boost::thread::hardware_concurrency();
    if (replicationNumber%maxThreadNumber>0) {
        threadNumber=5;
        if (replicationNumber%threadNumber>0) {
            std::cout<<"Warning: the iteration number should be divisible by the thread number"<<'\n';
        }
    }
    else
        threadNumber=maxThreadNumber;
}
