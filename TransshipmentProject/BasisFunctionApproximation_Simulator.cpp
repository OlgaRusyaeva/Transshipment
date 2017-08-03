//
//  BasisFunctionApproximation_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#include "BasisFunctionApproximation_Simulator.h"
#include "iProblem.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ActionsGenerator.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"
#include "Demand.h"
#include "BFValueFunction.h"
#include "BasisFunction.h"

using namespace std;
using namespace actions;
using namespace arrays;

bool BasisFunctionApproximation_Simulator::timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)
{
    bool flag=false;
    bool isZero=false;
    
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    State state=currentState;
    Demand demand=problem.getDemand();
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed+currentPeriod+20);
    
    float expectedProfitNextPeriodsWithCurrentTransshipment=getExpectedProfitWithCurrentTransshipment(cost, currentPeriod, state,isZero);
    
    float expectedProfitNextPeriodWithTransshipments=getExpectedProfitNextPeriodsWithTransshipments(currentPeriod, currentState);
    
    if((!isZero)&&(expectedProfitNextPeriodsWithCurrentTransshipment>=expectedProfitNextPeriodWithTransshipments))
    {
        flag=true;
    }
    
    
    return flag;
    
}

float BasisFunctionApproximation_Simulator::getExpectedProfitNextPeriodsWithTransshipments(const int& currentPeriod, const State& state)
{
    BasisFunction bf(state);
    
    float value=0;
    for (int i=0; i<(int)bf.getSize(); i++) {
        value+=bf.get()[i]*VF->get()[currentPeriod][i];
    }
    value+=VF->get()[currentPeriod][(int)bf.getSize()];
    
    return value;
}


void BasisFunctionApproximation_Simulator::initializeValueFunction(const BFValueFunction& valueFunction)
{
    VF=new BFValueFunction();
    *VF=valueFunction;
    
}


BasisFunctionApproximation_Simulator::~BasisFunctionApproximation_Simulator()
{
    delete VF;
}