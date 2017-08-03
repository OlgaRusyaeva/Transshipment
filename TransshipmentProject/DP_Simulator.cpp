//
//  DP_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "DP_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ValueFunction.h"
#include "Hashing.h"
#include "ActionsGenerator.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"
#include "Demand.h"
#include "iProblem.h"

using namespace std;
using namespace actions;
using namespace arrays;

/*DP_SimulatorPost::~DP_SimulatorPost()
{
    delete VF;
    delete HF;
}
    
void DP_SimulatorPost::initializeValueFunction(const ValueFunction& valueFunction)
{
	VF=new ValueFunction();
    *VF=valueFunction;
}

void DP_SimulatorPost::initializeHashFunction(const Hashing& hashFunction)
{
	HF=new Hashing();
    *HF=hashFunction;
}

float DP_SimulatorPost::getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
        vecMatrixInt actionsForState=transhipmentFromState(state,false); 
        
        State indexVector(location);
        int size_actions=1;
        simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
    	arrays::Array2D<float> combinationsOfRowsActionMatrix(location, size_actions);
        combinationsOfRowsActionMatrix.initialize(); 
        simulationMethods::generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);              
        
    	float maxCost=-std::numeric_limits<float>::max();
        State maxState(location);
        
        for (int p=0; p<combinationsOfRowsActionMatrix.sizeH(); p++) {
            
            float costInPeriod =0;
            State newState(location);
            Actions transshipmentMatrix(location);
            
            newState=state;
            for (int j=0; j<=location-1; j++) {
                int sumTransshipAmountFromJ=0;
                
                for (int i=0; i<=location-1; i++) {
                    int amountToTransship=0;
                    if(i!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][i];
                    newState.setElement(i,newState.getElement(i)+amountToTransship);
                    sumTransshipAmountFromJ+=amountToTransship;
                    transshipmentMatrix.setElement(j,i,amountToTransship);
                    
                }
                newState.setElement(j,newState.getElement(j)-sumTransshipAmountFromJ);
            }
            costInPeriod-=problemMethods::getTransshipmentCost(cost, transshipmentMatrix);
            costInPeriod+=VF->getValueFunction(currentPeriod+1, HF->hash_value(newState));
            
            if (costInPeriod>=maxCost) {
                maxCost=costInPeriod;
                maxState=newState;
                maxTransshipmentMatrix=transshipmentMatrix;
            }
            
        }
        
        state=maxState;
        
        return problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);  
        
        
}*/

DP_Simulator::~DP_Simulator()
{
    delete VF;
    delete HF;
}

void DP_Simulator::initializeValueFunction(const ValueFunction& valueFunction)
{
	VF=new ValueFunction();
    *VF=valueFunction;
}

void DP_Simulator::initializeHashFunction(const Hashing& hashFunction)
{
	HF=new Hashing();
    *HF=hashFunction;
}
    
float DP_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{
    int location = problem.getStateDimension();
    int lifeTime = problem.getLifeTime();
    Cost cost = problem.getCost();
    
    
    vector<vecMatrixInt> actionsForLifeTimeForLocation(lifeTime);
    vector<boost::shared_ptr<Array2D<float> > > combinationsofRowsForLifeTimeForLocation(lifeTime);
    
    State totalIndexVector(lifeTime);
    int totalSize=1;
    for (int m=0; m<lifeTime; m++) {
        actionsForLifeTimeForLocation[m]=transhipmentFromState(currentState,m,false);
        
        State indexVector(location);
        int size_actions=1;
        
        simulationMethods::getIndexVector(indexVector, actionsForLifeTimeForLocation[m],size_actions);
        
        boost::shared_ptr<Array2D<float> > combinationsOfRowsActionMatrix(new Array2D<float>(location, size_actions));
        combinationsOfRowsActionMatrix->initialize();
        simulationMethods::generatePossibleDemands(indexVector,*combinationsOfRowsActionMatrix);
        combinationsofRowsForLifeTimeForLocation[m]=combinationsOfRowsActionMatrix;
        
        totalSize*=size_actions;
        totalIndexVector.setElement(m,0,size_actions-1);
        
    }
    
    
    float maxProfit=-std::numeric_limits<float>::max();
    State maxState(location,1,lifeTime);
    
    Demand demand(location);
    
    Array2D<float> combinationsOfRowsForLifeTime(lifeTime, totalSize);
    combinationsOfRowsForLifeTime.initialize();
    simulationMethods::generatePossibleDemands(totalIndexVector,combinationsOfRowsForLifeTime);

    for (int s=0; s<combinationsOfRowsForLifeTime.sizeH(); s++) {
        
        float profitInPeriod =0;
        State newState(location,1,lifeTime);
        Actions transshipmentMatrix(location);
        newState=currentState;
        for (int m=0; m<lifeTime; m++) {
            int p=(int)combinationsOfRowsForLifeTime(s,m);
            for (int j=0; j<=location-1; j++) {
            	for (int i=0; i<=location-1; i++) {
                	int amountToTransship=0;
                    int index=(int)combinationsofRowsForLifeTimeForLocation[m]->operator()(p,j);
                	if(i!=j) amountToTransship=actionsForLifeTimeForLocation[m][j][index][i];
                    newState.setElement(i,m,newState.getElement(i,m)+amountToTransship);
                	transshipmentMatrix.setElement(j,i,transshipmentMatrix.getElement(j,i)+amountToTransship);
                	newState.setElement(j,m,newState.getElement(j,m)-amountToTransship);
                }
                
            }

        }
        profitInPeriod-=problemMethods::getTransshipmentCost(cost, transshipmentMatrix);
        
        State minBorder(location);
        int size_demands=1;
        for (int i=0; i<location; i++) {
            for (int m=0; m<lifeTime; m++) {
            	minBorder.setElement(i,0, minBorder.getElement(i,0)+newState.getElement(i,m));
            }
            size_demands*=minBorder.getElement(i,0)+1;
        }
        Array2D<float> possibleDemands(location,size_demands);
        possibleDemands.initialize();
        simulationMethods::generatePossibleDemands(minBorder,possibleDemands); 
        
        
        for (int i=0; i<possibleDemands.sizeH(); i++) 
        {
            //calculate probability
            float prob=1;
            bool flag;
            for (int j=0; j<location; j++) {
                flag=true;
                demand.setElement(j, (int)possibleDemands(i,j));
                int sumInventory=0;
                for (int m=0; m<lifeTime; m++) {
                    sumInventory+=newState.getElement(j,m);
                }
                if ((sumInventory-demand.getElement(j))==0) {
                    flag=false;
                }
                prob*=problemMethods::probabilityOfDemand(demand.getElement(j),distribution[j],flag);
            }
            
            //get reward
            float reward = problemMethods::getReward(cost.getSalesCost(),newState,demand); 
            
            State stateAfterDemandSatisfaction(location,1,lifeTime);
            stateAfterDemandSatisfaction=newState;
            
            problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
            
            //get holding cost
            float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
            
            problemMethods::getNewState(stateAfterDemandSatisfaction);
            
            float valueNextPeriod=0;
            
            //std::cout<<"hash: "<<HF->hash_value(stateAfterDemandSatisfaction)<<'\n';
            valueNextPeriod=VF->getValueFunction(currentPeriod+1, HF->hash_value(stateAfterDemandSatisfaction));
            
            //get value function
            
            profitInPeriod+=prob*(reward-holdingCost+valueNextPeriod);
            
        }
        
        if (profitInPeriod>=maxProfit) {
            maxProfit=profitInPeriod;
            maxState=newState;
            maxTransshipmentMatrix=transshipmentMatrix;
        }
        
    }
    
    currentState=maxState;
        
    
    //}
    return problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);  
    
    
}


    
