



//
//  ADP.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ADP.h"

void solveByADP(const int& location, const int& time, const long& numberOfIterations, State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters,ValueFunction& valueFunction, Hashing& hashFunction, matrixInt_t& matrixOfClusters)
{
    //Hashing hashFunction;
    const float epsilon=0.2;
    
    for (int j=0; j<location; j++) {
        outputClusters(location,distanceMatrix,matrixOfClusters[j]);
    }
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,20);
    
    vector_t maxDemandForDistribution = getMaxDemandForDistribution(distribution,location);
    
    vector<size_t> hashVector=vector<size_t>(location);
    
    for (int n=0; n <numberOfIterations;n++)
    {
        State state(location);
        for (int j=0; j<=location-1; j++) 
        {
            state.setElement(j,rand()%initialState.getElement(j)+1);
            
        }
        
        for (int t=0; t<time; t++) 
        {
            Demand demand(location);
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
            }
            
            for (int k=0; k<location; k++) {
                hashVector[k]=hashFunction.hash_value(getClusteredState(state,k+1,matrixOfClusters[k]));
            }
            
            float reward = getReward(cost.getSalesCost(),state,demand); 
            
            state=getNewState(state, demand);//state after demand satisfaction
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), state);
            
            vector_t stock_out = getShortageLocations(state,maxDemandForDistribution);
            vector_t left_over = getExcessLocations(state,maxDemandForDistribution);
            
            if ((t!=time-1)&&((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
                
                list_t requiredQuantity = calculateRequiredQuantity(stock_out,state,maxDemandForDistribution);
                list_t availableQuantity = calculateAvailableQuantity(left_over,state,maxDemandForDistribution);
                
                double random =randomDoubleNumber(0, 1);
                
                if (random <= epsilon) 
                {
                    Actions transshipmentMatrix2(location);
                    while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
                        
                        requiredQuantity.sort(MyDataSortPredicateDescending);
                        
                        row_t distanceToFirstShortageElement=distanceMatrix.getElementsForState(requiredQuantity.front().first);
                        sortAvailableQuantity(availableQuantity,distanceToFirstShortageElement);
                        
                        int amountToTransship = getMinValue(availableQuantity.front().second, requiredQuantity.front().second);
                        state.setElement(requiredQuantity.front().first, state.getElement(requiredQuantity.front().first)+amountToTransship);
                        state.setElement(availableQuantity.front().first, state.getElement(availableQuantity.front().first)-amountToTransship);
                        transshipmentMatrix2.setElement(availableQuantity.front().first,requiredQuantity.front().first,amountToTransship);
                        
                        if ((amountToTransship!=availableQuantity.front().second)||(amountToTransship!=requiredQuantity.front().second)) {
                            if (amountToTransship!=availableQuantity.front().second) {
                                //add available - amount to availableQuantity
                                availableQuantity.push_back(make_pair(availableQuantity.front().first, availableQuantity.front().second-amountToTransship));
                            }
                            else {
                                // add required - amount to requiredQuantity
                                requiredQuantity.push_back(make_pair(requiredQuantity.front().first, requiredQuantity.front().second-amountToTransship));
                            }
                        }
                        requiredQuantity.pop_front();
                        availableQuantity.pop_front();  
                        
                    }
                }
                else
                {
                
                vecMatrixInt actionsForState=generateActionsForEachLocation(requiredQuantity,availableQuantity,location);   
                
                State indexVector(location);
                int size_actions=1;
                getIndexVector(indexVector, actionsForState,size_actions);
                Array2DInt<float> combinationsOfRowsActionMatrix(location, size_actions);
                combinationsOfRowsActionMatrix.initialize();    
                generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);
                
                //for each possible action calulate cost
                float maxCost=-numeric_limits<float>::max();
                State maxState(location);
                    Actions maxTransshipmentMatrix(location);
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
                    costInPeriod-=holdingCost;
                    costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                    
                    float costOfValueFunction=0;
                    for (int i=0; i<location; i++) {
                        costOfValueFunction+=((i+1)/location)*valueFunction.getValueFunction(t+1, hashFunction.hash_value(getClusteredState(newState,i+1,matrixOfClusters[i])),i);
                    }
                    costInPeriod+=costOfValueFunction;                   
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState=newState;
                        maxTransshipmentMatrix=transshipmentMatrix;
                        /*for (int i=0; i<location; i++) {
                            for (int j=0; j<location; j++) {
                                maxTransshipmentMatrix.setElement(i,j,transshipmentMatrix.getElement(i,j));
                            }
                        }*/
                    }
                    
                }
                    state=maxState;

                float updateCost=0;
                for (int i=0; i<location; i++) {
                    updateCost=(1-(i+1)*0.1)*valueFunction.getValueFunction(t, hashVector[i],i)+(i+1)*0.1*(reward+maxCost);
                    valueFunction.setValueFunction(t,hashVector[i],i,updateCost); 
                }
                
                }
            }
            else
            {
                float updateCost=0;
                for (int i=0; i<location; i++) {
                    updateCost=(1-(i+1)*0.1)*valueFunction.getValueFunction(t, hashVector[i],i)+(i+1)*0.1*(reward-holdingCost);
                    valueFunction.setValueFunction(t,hashVector[i],i,updateCost); 
                }
            }
            
        }
        
    }
    
}

float getTransshipmentCostByADP(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,const Demand& demand, const int& currentPeriod, vector<boost::shared_ptr<iDistribution> > distribution, ValueFunction& valueFunction,Actions& maxTransshipmentMatrix,Hashing& hashFunction,const matrixInt_t& matrixOfClusters)
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
        getIndexVector(indexVector, actionsForState,size_actions);
        Array2DInt<float> combinationsOfRowsActionMatrix(location, size_actions);
        combinationsOfRowsActionMatrix.initialize(); 
        generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);
        
        //for each possible action calulate cost
        float maxCost=-numeric_limits<float>::max();
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
            
            costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
            
            float costOfValueFunction=0;
            for (int i=0; i<location; i++) {
                costOfValueFunction+=((i+1)/location)*valueFunction.getValueFunction(currentPeriod+1, hashFunction.hash_value(getClusteredState(newState,i+1,matrixOfClusters[i])),i);//((i+1)/location)
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
    float transshipmentCost = getTransshipmentCost(cost.getTransshipmentCost(),distanceMatrix,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}

/*------------------------------------------------Comment---------------------------------------*/

/*row_t evaluationADPPolicy(const ValueFunction& valueFunction, Hashing& hashFunction,const int& location, const int& time, const long& numberOfIterations, State& initialState,const Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters, const matrixInt_t& matrixOfClusters)
{
    
    row_t profit=row_t(numberOfIterations);
    int transshipmentSize=0;
    matrixInt_t count = initializeTransshipmentMatrix(location,location);
    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    vector_t maxDemandForDistribution = getMaxDemandForDistribution(distribution,location);
    
    for (int n=0; n<numberOfIterations; n++) 
    {
        //cout<<"---------------------nn-----------------"<<n<<'\n';
        float totalProfit = 0;
        State state(location);
        state.setState(initialState);
        
        for (int t=0; t<time; t++) 
        {
            
            //cout<<"Demand: "<<'\n';
            Demand demand(location);
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)(ceil(distribution[i]->sample())));
                //cout<<demand->getElement(i)<<'\n';
            }
            
            float reward = getReward(cost.getSalesCost(),state,demand); 
            
            state.setState(getNewState(state, demand));
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), state);
            
            //delete demand;
            
            vector_t stock_out = getShortageLocations(state,maxDemandForDistribution);
            vector_t left_over = getExcessLocations(state,maxDemandForDistribution);
            
            if ((t!=time-1)&&((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
                
                list_t requiredQuantity = calculateRequiredQuantity(stock_out,state,maxDemandForDistribution);
                list_t availableQuantity = calculateAvailableQuantity(left_over,state,maxDemandForDistribution);
                
                vecMatrixInt actionsForState=generateActionsForEachLocation(requiredQuantity,availableQuantity,location); //reduced 2 times
                
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
                    
                    costInPeriod-=holdingCost;
                    costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                    
                    float costOfValueFunction=0;
                    for (int i=0; i<location; i++) {
                        costOfValueFunction+=((i+1)/location)*valueFunction.getValueFunction(t+1, hashFunction.hash_value(getClusteredState(newState,i+1,matrixOfClusters[i])),i);//((i+1)/location)
                    }
                    costInPeriod+=costOfValueFunction;
                    
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
                
                totalProfit+=reward-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix)-holdingCost;  
                
            }
            else
                totalProfit+=reward-holdingCost;
        }
        
        //delete state;
        profit[n]=totalProfit;
    }
    //std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    
    cout<<"Total amount of stock movement: "<<'\n';
    printMatrix(location, location, count);
    cout<<"average transshipment size: "<<(float)(transshipmentSize/numberOfIterations)<<'\n';
    
    return profit;
    
    
}*/



