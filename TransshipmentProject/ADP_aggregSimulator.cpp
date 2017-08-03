//
//  ADP_aggregSimulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ADP_aggregSimulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ValueFunction.h"
#include "Hashing.h"
#include "RangingMethods.h"
#include "Clustering.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"

using namespace std;
using namespace rangingMethods;
using namespace clustering;

    
ValueFunction& ADP_aggregSimulator::getValueFunction() const
{
    return *VF;
}
    
Hashing& ADP_aggregSimulator::getHashFunction() const
{
    return *HF;
}

    
void ADP_aggregSimulator::initializeValueFunction()
{
    VF = new ValueFunction();
}
    
void ADP_aggregSimulator::initializeHashFunction()
{
    HF = new Hashing();
}

    
 float ADP_aggregSimulator::getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost, const Actions& distanceMatrix,vector<boost::shared_ptr<iDistribution> > distribution, Actions& maxTransshipmentMatrix)
{
        vector_t maxDemandForDistribution = getMaxDemandForDistribution(distribution,location);
        
        vector_t stock_out = getShortageLocations(state,maxDemandForDistribution);
        vector_t left_over = getExcessLocations(state,maxDemandForDistribution);
        
        if (((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) 
        {  
            
            list_t requiredQuantity = calculateRequiredQuantity(stock_out,state,maxDemandForDistribution);
            list_t availableQuantity = calculateAvailableQuantity(left_over,state,maxDemandForDistribution);
            
            vecMatrixInt actionsForState=generateActionsForEachLocation(requiredQuantity,availableQuantity,location); //reduced 2 times
            
            State indexVector(location);
            int size_actions=1;
            simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
            arrays::Array2DInt<float> combinationsOfRowsActionMatrix(location, size_actions);
            combinationsOfRowsActionMatrix.initialize(); 
            simulationMethods::generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);
            
            //for each possible action calulate cost
            float maxCost=-std::numeric_limits<float>::max();
            State maxState(location);
            
            for (int p=0; p<combinationsOfRowsActionMatrix.size(); p++) {
                float costInPeriod =0;
                Actions transshipmentMatrix(location);
                State newState(location);
                newState=state;
                for (int j=0; j<=location-1; j++) {
                    int sumTransshipAmountFromJ=0;
                    for (int k=0; k<=location-1; k++) {
                        int amountToTransship=0;
                        if(k!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][k];
                        sumTransshipAmountFromJ+=amountToTransship;
                        newState.setElement(k,newState.getElement(k)+amountToTransship);
                        transshipmentMatrix.setElement(j,k,amountToTransship);
                        
                    }
                    newState.setElement(j,newState.getElement(j)-sumTransshipAmountFromJ);
                    
                }
                
                costInPeriod-=problemMethods::getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                
                float costOfValueFunction=0;
                for (int i=0; i<location; i++) {
                    costOfValueFunction+=((i+1)/location)*VF->getValueFunction(currentPeriod+1, HF->hash_value(getClusteredState(newState,i+1,VF->getCluster(i))),i);//((i+1)/location)
                }
                costInPeriod+=costOfValueFunction;
                
                if (costInPeriod>=maxCost) {
                    maxCost=costInPeriod;
                    maxState=newState;
                    for (int i=0; i<location; i++) {
                        for (int j=0; j<location; j++) {
                            maxTransshipmentMatrix.setElement(i,j,transshipmentMatrix.getElement(i,j));
                        }
                    }
                }
                
            }
            
            state=maxState;
            
            
        }
    float transshipmentCost = problemMethods::getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}
    
    
