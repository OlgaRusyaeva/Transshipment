//
//  ADPForwardMultipleTransshipments_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 04/03/15.
//
//

#include "ADPForwardMultipleTransshipments_Solver.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "Demand.h"
#include "ProblemMethods.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ActionsGenerator.h"
#include "ValueFunction.h"
#include "Hashing.h"


float ADPForwardMultipleTransshipments_Solver::getCostForActions(State& state,const int& currentPeriod,const Cost& cost, const int& time, const arrays::Array2D<float>& possibleDemands)const
{
    
    const int size=state.getLength();
    
    Actions maxTransshipmentMatrix(size);
    
    vecMatrixInt actionsForState=actions::transhipmentFromState(state,0,false);
    
    State indexVector(size);
    int size_actions=1;
    
    simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
    
    
    arrays::Array2D<float> combinationsOfRowsActionMatrix(size, size_actions);
    combinationsOfRowsActionMatrix.initialize();
    simulationMethods::generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);
    
    float maxCost=-std::numeric_limits<float>::max();
    State maxState(size);
    
    
    Demand demand(size);
    
    /*cout<<"possible demand sample: "<<'\n';
     for (int i=0; i<possibleDemands.sizeH(); i++) {
     for (int j=0; j<possibleDemands.sizeW(); j++) {
     cout<<possibleDemands(i,j)<<" ";
     }
     cout<<'\n';
     }*/
    
    for (int p=0; p<combinationsOfRowsActionMatrix.sizeH(); p++) {
        
        //cout<<"Action number: "<<p<<'\n';
        
        State newState(size);
        Actions transshipmentMatrix(size);
        
        float costInPeriod =0;
        
        newState=state;
        for (int j=0; j<=size-1; j++) {
            int sumTransshipAmountFromJ=0;
            
            for (int i=0; i<=size-1; i++) {
                int amountToTransship=0;
                if(i!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][i];
                newState.setElement(i,0,newState.getElement(i,0)+amountToTransship);
                sumTransshipAmountFromJ+=amountToTransship;
                transshipmentMatrix.setElement(j,i,amountToTransship);
                
            }
            newState.setElement(j,0,newState.getElement(j,0)-sumTransshipAmountFromJ);
        }
        
        /*cout<<"Current transshipment: "<<'\n';
         for (int i=0; i<size; i++) {
         for (int j=0; j<size; j++) {
         cout<<transshipmentMatrix.getElement(i,j)<<" ";
         }
         cout<<'\n';
         }*/
        
        /*cout<<"Post-decision state: "<<'\n';
         for (int i=0; i<size; i++) {
         cout<<newState.getElement(i)<<'\n';
         }*/
        
        costInPeriod-=problemMethods::getTransshipmentCost(cost, transshipmentMatrix);
        
        //cout<<"transshipment cost: "<<problemMethods::getTransshipmentCost(cost, transshipmentMatrix)<<'\n';
        
        for (int d=0; d<possibleDemands.sizeH(); d++) {
            //generate demand
            
            //cout<<"Demand: "<<'\n';
            for (int l=0; l<size; l++) {
                demand.setElement(l, possibleDemands(d,l));
                //probability*=distribution[l]->pdf(demand.getElement(l));
                // std::cout<<demand.getElement(l)<<'\n';
            }
            
            float reward = problemMethods::getReward(cost.getSalesCost(),newState,demand);
            //cout<<"reward: "<<reward<<'\n';
            
            State stateAfterDemandSatisfaction(size);
            stateAfterDemandSatisfaction=newState;
            
            problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
            
            /*cout<<"State after demand satisfaction"<<'\n';
             for (int i=0; i<size; i++) {
             cout<<stateAfterDemandSatisfaction.getElement(i)<<'\n';
             }*/
            
            //get holding cost
            float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
            //cout<<"Holding cost:"<<holdingCost<<'\n';
            
            float valueNextPeriod=0;
            
            // cout<<"Hash of state after demand satisfaction: "<<hashFunction->hash_value(stateAfterDemandSatisfaction)<<'\n';
            valueNextPeriod=valueFunction->getValueFunction(currentPeriod+1, hashFunction->hash_value(stateAfterDemandSatisfaction));
            
            //cout<<"Value of being in next state: "<<valueNextPeriod<<'\n';
            
            //cout<<"One of possible costs: "<< ((float)1/demandNumber)*(reward-holdingCost+valueNextPeriod)<<'\n';
            //cout<<reward-holdingCost+valueNextPeriod<<'\n';
            
            costInPeriod+=((float)1/demandNumber)*(reward-holdingCost+valueNextPeriod);
            
        }
        
        
        if (costInPeriod>=maxCost) {
            maxCost=costInPeriod;
            maxState=newState;
            maxTransshipmentMatrix=transshipmentMatrix;
        }
        
    }
    
    state=maxState;
    //problemMethods::getNewState(state,maxTransshipmentMatrix);
    
    //cout<<"Max cost: "<<maxCost<<'\n';
    
    return maxCost;
    
}
