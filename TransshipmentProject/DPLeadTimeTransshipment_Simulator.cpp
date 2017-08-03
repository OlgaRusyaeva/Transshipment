//
//  DPLeadTimeTransshipment_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#include "DPLeadTimeTransshipment_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ValueFunction.h"
#include "Hashing.h"
#include "Demand.h"
#include "iProblem.h"
#include "ActionsGenerator.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"
#include "LeadTimes.h"


using namespace std;
using namespace actions;
using namespace arrays;

DPLeadTimeTransshipment_Simulator::~DPLeadTimeTransshipment_Simulator()
{
    delete VF;
    delete HF;
}

void DPLeadTimeTransshipment_Simulator::initializeValueFunction(const ValueFunction& valueFunction)
{
    VF=new ValueFunction();
    *VF=valueFunction;
}

void DPLeadTimeTransshipment_Simulator::initializeHashFunction(const Hashing& hashFunction)
{
    HF=new Hashing();
    *HF=hashFunction;
}

void DPLeadTimeTransshipment_Simulator::initializeInitialState(const boost::shared_ptr<State>& state)
{
    IL=state;
}

float DPLeadTimeTransshipment_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    LeadTimes leadTimes=problem.getLeadTimes();
    
    vecMatrixInt actionsForState=transhipmentFromState(currentState,0,false);
    
    State indexVector(location);
    int size_actions=1;
    
    simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
    
    //if (((float)size_actions*(float)location<numeric_limits<int>::max())&&((float)size_actions*(float)location>0)) {
    
    arrays::Array2D<float> combinationsOfRowsActionMatrix(location, size_actions);
    combinationsOfRowsActionMatrix.initialize();
    simulationMethods::generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);
    
    float maxProfit=-std::numeric_limits<float>::max();
    State maxState(location,leadTimes.getMaxLeadTimePlusOne());
    
    Demand demand(location);
    
    for (int p=0; p<combinationsOfRowsActionMatrix.sizeH(); p++) {
        
        float profitInPeriod =0;
        State newState(location,leadTimes.getMaxLeadTimePlusOne());
        Actions transshipmentMatrix(location);
        newState=currentState;
        problemMethods::getNewState(newState);
        for (int j=0; j<=location-1; j++) {
            for (int i=0; i<=location-1; i++) {
                int amountToTransship=0;
                if(i!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][i];
                newState.setElement(i,leadTimes.getLeadTimeBetweenLocations(j,i),newState.getElement(i,leadTimes.getLeadTimeBetweenLocations(j,i))+amountToTransship);
                transshipmentMatrix.setElement(j,i,amountToTransship);
                newState.setElement(j,0,newState.getElement(j,0)-amountToTransship);
                
            }
            
        }
        profitInPeriod-=problemMethods::getTransshipmentCost(cost, transshipmentMatrix);

        profitInPeriod+=VF->getValueFunction(currentPeriod+1, HF->hash_value(newState));
        if (profitInPeriod>=maxProfit) {
            maxProfit=profitInPeriod;
            maxState=newState;
            maxTransshipmentMatrix=transshipmentMatrix;
            //maxState=stateAfterDemandSatisfaction;
        }
    }
    currentState=maxState;

    return problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
}

State DPLeadTimeTransshipment_Simulator::getInitialState(iProblem& problem,const int& iteration) 
{
    return *IL;
}