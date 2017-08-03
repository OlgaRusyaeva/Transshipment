//
//  Policy Evaluation.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 4/29/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//


#include <iostream>
#include "PolicyEvaluation.h"
#include "Statistics.h"


row_t policyEvaluation(const int& location, const int& time, const long& numberOfIterations,State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters, ValueFunction& valueFunction, Hashing& hashFunction, matrixInt_t& indexVector, vecMatrixFloat& breakpoints, vecMatrixFloat& slope, matrixInt_t& matrixOfClusters, const int& typeOfMethod)
{
    
    assert((typeOfMethod<12)&&(typeOfMethod>0));
    
    row_t profit=row_t(numberOfIterations);
    int transshipmentSize=0;
    matrixInt_t count = initializeTransshipmentMatrix(location,location);
    Actions maxTransshipmentMatrix(location);
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    const vector_t& maxDemandForDistribution = getMaxDemandForDistribution(distribution,location);
    
    switch (typeOfMethod) {
        case 9:
            solveByADP_piecewise(location, time, numberOfIterations, initialState, cost, distanceMatrix, demandParameters, indexVector, breakpoints, slope);
            break;
        case 10:
            solveByADP(location, time, numberOfIterations, initialState, cost, distanceMatrix, demandParameters,valueFunction,hashFunction, matrixOfClusters);
            break;
        case 11:
            //solveExactDP(location, time, initialState, cost, distanceMatrix, demandParameters, valueFunction, hashFunction);
            valueFunction.inputVFunction();
            break;
        default:
            break;
    }
    
    State state(location);
    Demand demand(location);
    
    for (int n=0; n <numberOfIterations;n++)
    {
        int numberOfTransshipments=0;
        float totalProfit = 0;
        
        state=initialState;
        
        for (int t=0; t<time; t++) 
        {
           
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
            }
            
            float reward = getReward(cost.getSalesCost(),state,demand); 
            
            state=getNewState(state, demand);
            
            float holdingCost = getHoldingCost(cost.getHoldingCost(),state);
            
            if ((t!=time-1)&&(typeOfMethod!=1))
            {  
                totalProfit+=reward-holdingCost;
                
                float transshipmentCost=0;
                
                switch (typeOfMethod) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        transshipmentCost=gettransshipmentCostByReactiveMethod(location, time, state, cost, distanceMatrix,distribution,typeOfMethod,maxTransshipmentMatrix); 
                        break;
                    case 6:
                        transshipmentCost=gettransshipmentCostByTBA(location, time, state, cost, distanceMatrix,distribution,t,maxTransshipmentMatrix);
                        break;
                    case 7:
                        transshipmentCost= gettransshipmentCostByTBA2(location, state, cost, distanceMatrix,distribution,t,maxDemandForDistribution,maxTransshipmentMatrix);;
                        break;
                    case 8:
                        transshipmentCost=gettransshipmentCostByTIE(location, state, cost, distanceMatrix,distribution,numberOfTransshipments,maxTransshipmentMatrix);
                        break;
                    case 9:
                        transshipmentCost=getTransshipmentCostByADP_piecewise(indexVector, breakpoints, slope, t, state, cost, distanceMatrix, maxTransshipmentMatrix);
                        break;
                    case 10:
                        transshipmentCost=getTransshipmentCostByADP(location,time, state, cost, distanceMatrix, demand, t, distribution, valueFunction, maxTransshipmentMatrix, hashFunction, matrixOfClusters);
                        break;
                    case 11:
                        transshipmentCost=getTransshipmentCostByExactDP(location, time, state, cost, distanceMatrix, demand, t, valueFunction,maxTransshipmentMatrix, hashFunction);
                        break;
 
                }
                
                
                if (transshipmentCost>0) {
                    numberOfTransshipments++;
                }
                
                totalProfit-=transshipmentCost;
                
                for (int i=0; i<location; i++) {
                    for (int j=0; j<location; j++) {
                        if (maxTransshipmentMatrix.getElement(i,j)!=0) {
                            transshipmentSize+=maxTransshipmentMatrix.getElement(i,j);
                            count[i][j]++;
                        }
                    }
                }

                
            }
            
            else
            {
                totalProfit+=reward-holdingCost; 
            }           
            
            
        }
        profit[n]=totalProfit;
        
    }
    
    cout<<"Total amount of stock movement: "<<'\n';
    printMatrix(location, location, count);
    cout<<"average transshipment size: "<<((float)(transshipmentSize/numberOfIterations))<<'\n';
    
    return profit;
    
}

