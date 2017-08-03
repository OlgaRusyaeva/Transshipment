//
//  ModifiedADP.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 9/11/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include <iostream>
#include "ADP.h"

row_t solveByModifiedADP(const int& location, const int& time, const long& numberOfIterations, State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,ValueFunction& valueFunction)
{
    Hashing hashFunction;
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,40);
    
    for (int n=0; n <100000;n++)//10000 iterations
    {
        //cout<<"-----------------------n----------------"<<n<<'\n';
        State state(location);
        for (int j=0; j<=location-1; j++) 
        {
            //state.setElement(j,rand()%initialState.getElement(j)+1);
            state.setElement(j,initialState.getElement(j));
            
        }
        
        for (int t=0; t<time; t++) 
        {
            Demand demand(location);
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
                    //cout<<demand.getElement(l)<<'\n';

            }
            size_t initialStateHash=0;
            initialStateHash = hashFunction.hash_value(state);

            float reward=0;
            reward = getReward(cost.getSalesCost(),state,demand); 

            State oldState(location);
            oldState.setState(state);
            
            state.setState(getNewState(state, demand));//state after demand satisfaction

            float holdingCost=0;
            holdingCost=getHoldingCost(cost.getHoldingCost(), state);
            
            if ((t!=time-1)) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
                vecMatrixInt actionsForState=generateActionsZeroOne(state, t,valueFunction, hashFunction, distanceMatrix, cost.getTransshipmentCost());  
            
                Array2DInt<float> combinationsOfRowsActionMatrix=generateAllCombinationsOfTransshipmentActions(actionsForState,location);
                    
                    //for each possible action calulate cost
                float maxCost=-numeric_limits<float>::max();
                State maxState(location);
                Actions maxTransshipmentMatrix(location);// = initializeTransshipmentMatrix(location,location);
                for (int p=0; p<combinationsOfRowsActionMatrix.size(); p++) {
                    float costInPeriod =0;
                    Actions transshipmentMatrix(location);// = initializeTransshipmentMatrix(location,location);
                    State newState(location);
                    newState.setState(state);
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
                    costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                     
                    /*if (t==time-2) {
                        float lastPeriodProfit=0;
                        for (int i=0; i<location; i++) {
                            lastPeriodProfit+=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-expectedValuePositivePart(distribution[i],newState.getElement(i)))-cost.getHoldingCost()[i]*newState.getElement(i);
                        }
                        valueFunction.setValueFunction(t+1, hashFunction.hash_value(newState),0, lastPeriodProfit);
                    }*/
                    
                    /*for (int i=0; i<location; i++) {
                        costInPeriod+=valueFunction.getValueFunction(t+1, newState.getElement(i),i);
                    }*/
                    
                    costInPeriod+=valueFunction.getValueFunction(t+1, hashFunction.hash_value(newState),0);
                                          
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState.setState(newState);
                        for (int i=0; i<location; i++) {
                            for (int j=0; j<location; j++) {
                                maxTransshipmentMatrix.setElement(i,j,transshipmentMatrix.getElement(i,j));
                            }
                        }
                    }
                        
                }
                state.setState(maxState);
                
                float updateCost=0;
                float alpha=(float)300/(300+n);
                updateCost=(1-alpha)*valueFunction.getValueFunction(t,initialStateHash,0)+alpha*(reward-holdingCost+maxCost);
                valueFunction.setValueFunction(t,initialStateHash,0,updateCost); 


                    
            }
            else
            {
                float updateCost=0;
                float alpha=(float)100/(100+n);
                updateCost=(1-alpha)*valueFunction.getValueFunction(t,initialStateHash,0)+alpha*(reward-holdingCost);
                valueFunction.setValueFunction(t,initialStateHash,0,updateCost); 
                /*float lastPeriodProfit=0;
                for (int i=0; i<location; i++) {
                    lastPeriodProfit+=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-expectedValuePositivePart(distribution[i],oldState.getElement(i)))-cost.getHoldingCost()[i]*oldState.getElement(i);
                }
                valueFunction.setValueFunction(t+1, initialStateHash,0, lastPeriodProfit);*/
            

            }
        }
            
    }
        
    
    return evaluationModifiedADPPolicy(valueFunction,hashFunction, location, time, numberOfIterations, initialState, cost, distanceMatrix, demandParameters);
    
}

row_t evaluationModifiedADPPolicy(const ValueFunction& valueFunction, Hashing& hashFunction,const int& location, const int& time, const long& numberOfIterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
{
    
    row_t profit=row_t(numberOfIterations);
    int transshipmentSize=0;
    matrixInt_t count = initializeTransshipmentMatrix(location,location);
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    for (int n=0; n<numberOfIterations; n++) 
    {
        cout<<"---------------------n-----------------"<<n<<'\n';
        float totalProfit = 0;
        State state(location);
        state.setState(initialState);
        
        for (int t=0; t<time; t++) 
        {
            cout<<"Time: "<<t<<'\n';
            cout<<"State: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<state.getElement(i)<<'\n';
            }
            cout<<"Demand: "<<'\n';
            Demand demand(location);
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)(ceil(distribution[i]->sample())));
                cout<<demand.getElement(i)<<'\n';
            }
            
            float reward = getReward(cost.getSalesCost(),state,demand); 
            
            state.setState(getNewState(state, demand));
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), state);
            
            //delete demand;
            
            
            if ((t!=time-1)) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
                
                vecMatrixInt actionsForState=generateActionsZeroOne(state, t,valueFunction, hashFunction, distanceMatrix, cost.getTransshipmentCost());  //,matrixOfClusters
                
                Array2DInt<float> combinationsOfRowsActionMatrix=generateAllCombinationsOfTransshipmentActions(actionsForState,location);
                
                //for each possible action calulate cost
                float maxCost=-numeric_limits<float>::max();
                State maxState(location);
                Actions maxTransshipmentMatrix(location);// = initializeTransshipmentMatrix(location,location);
                for (int p=0; p<combinationsOfRowsActionMatrix.size(); p++) {
                    float costInPeriod =0;
                    Actions transshipmentMatrix(location);// = initializeTransshipmentMatrix(location,location);
                    State newState(location);
                    newState.setState(state);
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
                    
                    costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                    costInPeriod+=valueFunction.getValueFunction(t+1, hashFunction.hash_value(newState),0);
                    
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState.setState(newState);
                        for (int i=0; i<location; i++) {
                            for (int j=0; j<location; j++) {
                                maxTransshipmentMatrix.setElement(i,j,transshipmentMatrix.getElement(i,j));
                            }
                        }
                    }
                    //delete newState;
                    
                }
                
                state.setState(maxState);
                //delete maxState;
                
                
                for (int i=0; i<location; i++) {
                    for (int j=0; j<location; j++) {
                        if (maxTransshipmentMatrix.getElement(i,j)!=0) {
                            //cout<<"i "<<i<<'\n';
                            //cout<<"j "<<j<<'\n';
                            //cout<<maxTransshipmentMatrix[i][j]<<'\n';
                            transshipmentSize+=maxTransshipmentMatrix.getElement(i,j);
                            count[i][j]++;
                        }
                    }
                }
                
                //cout<<"transshipment Matrix"<<'\n';
                //printMatrix(location, location, maxTransshipmentMatrix);
                cout<<"Total profit per period: "<<'\n';
                cout<<reward-holdingCost-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix)<<'\n';
                
                totalProfit+=reward-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix)-holdingCost;  
                
            }
            else
            {
                totalProfit+=reward-holdingCost;
                cout<<"Total profit per last period: "<<'\n';
                cout<<reward-holdingCost<<'\n';
            }
        }
        
        //delete state;
        profit[n]=totalProfit;
    }
    //std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    
    cout<<"Total amount of stock movement: "<<'\n';
    printMatrix(location, location, count);
    cout<<"average transshipment size: "<<(float)(transshipmentSize/numberOfIterations)<<'\n';
    
    return profit;
    
    
}
