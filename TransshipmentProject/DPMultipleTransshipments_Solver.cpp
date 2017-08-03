//
//  DPMultipleTransshipments_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 24/02/15.
//
//

#include "DPMultipleTransshipments_Solver.h"
#include "State.h"
#include "Actions.h"
#include "Demand.h"
#include "Cost.h"
#include "iDistribution.h"
#include "Hashing.h"
#include "ValueFunction.h"
#include "iProblem.h"
#include "ActionsGenerator.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"
#include <boost/shared_ptr.hpp>


using namespace std;
using namespace actions;
using namespace arrays;


float DPMultipleTransshipments_Solver::getMaxProfit(const int& t, const int& time, const State& state, Demand& demand, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution) const
{
    const int location=state.getLength();
    const int lifeTime=state.getWidth();
    
    float maxProfit=-numeric_limits<float>::max();
    
    vector<vecMatrixInt> actionsForLifeTimeForLocation(lifeTime);
    vector<boost::shared_ptr<Array2D<float> > > combinationsofRowsForLifeTimeForLocation(lifeTime);
    
    State totalIndexVector(lifeTime);
    int totalSize=1;
    for (int m=0; m<lifeTime; m++) {
        actionsForLifeTimeForLocation[m]=transhipmentFromState(state,m,false);
        
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

    Array2D<float> combinationsOfRowsForLifeTime(lifeTime, totalSize);
    combinationsOfRowsForLifeTime.initialize();
    simulationMethods::generatePossibleDemands(totalIndexVector,combinationsOfRowsForLifeTime);

    
    //State maxState(location);
    
    //row_t costInPeriods(combinationsOfRowsActionMatrix.size());
    //#pragma omp parallel for shared(costInPeriods,actionsForState,combinationsOfRowsActionMatrix)
    for (int s=0; s<combinationsOfRowsForLifeTime.sizeH(); s++) {
        float profitInPeriod =0;
        Actions transshipmentMatrix(location);
        State newState(location,1,lifeTime);
        newState=state;
        for (int m=0; m<lifeTime; m++) {
            int p=(int)combinationsOfRowsForLifeTime(s,m);
            for (int j=0; j<=location-1; j++) {
                for (int k=0; k<=location-1; k++) {
                    int amountToTransship=0;
                    int index=(int)combinationsofRowsForLifeTimeForLocation[m]->operator()(p,j);
                    if(k!=j) amountToTransship=actionsForLifeTimeForLocation[m][j][index][k];
                    newState.setElement(k,m,newState.getElement(k,m)+amountToTransship);
                    newState.setElement(j,m,newState.getElement(j,m)-amountToTransship);
                    transshipmentMatrix.setElement(j,k,transshipmentMatrix.getElement(j,k)+amountToTransship);
                    
                }
            }
        }
        profitInPeriod-=problemMethods::getTransshipmentCost(cost, transshipmentMatrix);
        
        //generate possible demands for state after making transshipments
        State minBorder(location);
        int size_demands=1;
        for (int i=0; i<location; i++) {
            for (int m=0; m<lifeTime; m++) {
                minBorder.setElement(i,0,minBorder.getElement(i,0)+newState.getElement(i,m));
            }
            size_demands*=minBorder.getElement(i,0)+1;
        }
        Array2D<float> possibleDemands(location,size_demands);
        possibleDemands.initialize();
        simulationMethods::generatePossibleDemands(minBorder,possibleDemands);
        
        /*cout<<"Possible demand:"<<'\n';
         for (int i=0; i<possibleDemands.size(); i++) {
         for (int j=0; j<location; j++) {
         cout<<possibleDemands(i,j)<<" ";
         }
         cout<<'\n';
         }*/
        
        //for each demand
        //float sumProb=0;
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
            //sumProb+=prob;
            
            //cout<<"Probability: "<<prob<<'\n';
            
            /*cout<<"Demand"<<'\n';
             for (int d=0; d<location; d++) {
             cout<<demand.getElement(d)<<'\n';
             }*/
            
            //get reward
            float reward = problemMethods::getReward(cost.getSalesCost(),newState,demand);
            //cout<<"reward: "<<reward<<'\n';
            
            State stateAfterDemandSatisfaction(location,1,lifeTime);
            stateAfterDemandSatisfaction=newState;
            
            problemMethods::getNewState(stateAfterDemandSatisfaction, demand);//!!!
            
            //get holding cost
            float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
            //cout<<"Holding cost:"<<holdingCost<<'\n';
            
             problemMethods::getNewState(stateAfterDemandSatisfaction);
            
            float valueNextPeriod=0;
            
            //std::cout<<"hash: "<<hashFunction->hash_value(stateAfterDemandSatisfaction)<<'\n';
            valueNextPeriod=valueFunction->getValueFunction(t+1, hashFunction->hash_value(stateAfterDemandSatisfaction));
            
            //cout<<"value next period: "<<valueNextPeriod<<'\n';
            //get value function
            
            profitInPeriod+=prob*(reward-holdingCost+valueNextPeriod);
            
        }
        //cout<<"SumProb: "<<sumProb<<'\n';
        
        if (profitInPeriod>=maxProfit) {
            maxProfit=profitInPeriod;
            //maxState=stateAfterDemandSatisfaction;
        }
        //costInPeriods[p]=costInPeriod;
        
    }
    
    return maxProfit;
    
}