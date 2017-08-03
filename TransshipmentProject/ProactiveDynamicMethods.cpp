//
//  ServiceLevelMethod.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 10/30/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ProactiveDynamicMethods.h"
#include "ReactiveMethods.h"


row_t solveByServiceLevelMethodReactive(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
{
    row_t profit=row_t(numberOfIterations);
    
    vector<iDistribution*> distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    vector_t maxDemandForDistribution = getMaxDemandForDistribution(distribution,location);
    
    for (int n=0; n <numberOfIterations;n++)
    {
        float totalProfit = 0;
        State* state = new State(location);
        state->setState(initialState);
        
        for (int t=0; t<time; t++) 
        {

            Demand *demand= new Demand(location);
            for (int i=0; i<location; i++) {
                demand->setElement(i, (int)distribution[i]->sample());
            }
            
            float reward = getReward(cost.getSalesCost(),*state,*demand); 
            
            state->setState(getNewState(*state, *demand));//state after demand satisfaction
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), *state);
            
            delete demand;
            
            vector_t stock_out=getStockOutLocations(*state);
            
            if ((t!=time-1)&&((int)stock_out.size()!=location)&&((int)stock_out.size()!=0)) // for last time interval [T-1,T] there is no sence to make a transshipment//here reactive case
            {  
                vecMatrixInt actionsForState=transhipmentFromState(*state,true); 
                            
                matrixInt_t combinationsOfRowsActionMatrix=generateAllCombinationsOfTransshipmentActions(actionsForState,location);
                
                //for each possible action calulate cost
                float maxCost=-numeric_limits<float>::max();
                State *maxState = new State(location);
                matrixInt_t maxTransshipmentMatrix = initializeTransshipmentMatrix(location,location);
                for (int p=0; p<(int)combinationsOfRowsActionMatrix.size(); p++) {
                    float costInPeriod =0;
                    matrixInt_t transshipmentMatrix = initializeTransshipmentMatrix(location,location);
                    State *newState = new State(location);
                    newState->setState(*state);
                    for (int j=0; j<=location-1; j++) {
                        int sumTransshipAmountFromJ=0;
                        for (int k=0; k<=location-1; k++) {
                            int amountToTransship=0;
                            if(k!=j) amountToTransship=actionsForState[j][combinationsOfRowsActionMatrix[p][j]][k];
                            //cost-=costInState.getTransshipmentCost()*actions.getElement(j,k)*actionsForState[j][combinationsOfRowsActionMatrix[p][j]][k];
                            sumTransshipAmountFromJ+=amountToTransship;
                            newState->setElement(k,newState->getElement(k)+amountToTransship);
                            transshipmentMatrix[j][k]=amountToTransship;
                            
                        }
                        newState->setElement(j,newState->getElement(j)-sumTransshipAmountFromJ);
                    
                    }
                    
                    State *minBorder = new State(location);
                    *minBorder=getDemandBorderForState(*newState,maxDemandForDistribution);
                    
                    matrixInt_t possibleDemands =generatePossibleDemands(*minBorder); 
                    float expectedTotalProfit=0;
                    
                    for (int i=0; i<=(int)(possibleDemands.size()-1); i++) 
                    {
                        Demand *demand= new Demand(location);
                        State *oldState = new State(location);
                        oldState->setState(*newState);
                        
                        float prob=1;
                        for (int j=0; j<location; j++) {
                            bool flag=true;
                            demand->setElement(j, possibleDemands[i][j]);
                            if ((oldState->getElement(j)-demand->getElement(j)==0)) {
                                flag=false;
                                prob*=probabilityOfDemand(demand->getElement(j),distribution[j],flag);
                            }
                            else
                                prob*=probabilityOfDemand(demand->getElement(j),distribution[j],flag);
                        }
                        
                        float nextReward = getReward(cost.getSalesCost(),*oldState,*demand); 
                        
                        oldState->setState(getNewState(*oldState, *demand));
                        
                        float nextHoldingCost=getHoldingCost(cost.getHoldingCost(), *oldState);
                        
                        float error = getErrorCost(distribution, *oldState, distanceMatrix,cost.getTransshipmentCost());
                        
                        expectedTotalProfit+=prob*(nextReward-nextHoldingCost-error);
                        
                        delete demand;
                        
                        delete oldState;
                        
                    }

                    costInPeriod=-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix)+expectedTotalProfit;
                    
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState->setState(*newState);
                        for (int i=0; i<location; i++) {
                            for (int j=0; j<location; j++) {
                                maxTransshipmentMatrix[i][j]=transshipmentMatrix[i][j];
                            }
                        }
                    }
                    delete newState;
                    
                }
                    state->setState(*maxState);
                    delete maxState;
                totalProfit+=reward-holdingCost-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix);
                
                
            }
            else
                totalProfit+=reward-holdingCost;
            

        }
            
        delete state; 
        profit[n]=totalProfit;
        }
    std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    
    //return (float)totalProfit/(float)numberOfIterations;
    return getStatistics(profit);
        
}


row_t solveByServiceLevelMethodProactiveUsingReactiveMethods(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
{
    row_t profit=row_t(numberOfIterations);
    
    vector<iDistribution*> distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    for (int n=0; n <numberOfIterations;n++)
    {
        float totalProfit = 0;
        State* state = new State(location);
        state->setState(initialState);
        
        //generate the whole demand in advance
        matrixInt_t allDemandForIteration = generateDemandForIteration(location,time, distribution);
        
        for (int t=0; t<time; t++) 
        {
            Demand *demand= new Demand(location);
            for (int i=0; i<location; i++) {
                demand->setElement(i, allDemandForIteration[t][i]);
            }
            
            float reward = getReward(cost.getSalesCost(),*state,*demand); 
            
            state->setState(getNewState(*state, *demand));//state after demand satisfaction
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), *state);

            delete demand;
            
            if (t!=time-1) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
                vecMatrixInt actionsForState=transhipmentFromState(*state,false); 
                
                matrixInt_t combinationsOfRowsActionMatrix=generateAllCombinationsOfTransshipmentActions(actionsForState,location);                  

                //for each possible action calulate cost
                float maxCost=-numeric_limits<float>::max();
                State *maxState = new State(location);
                matrixInt_t maxTransshipmentMatrix = initializeTransshipmentMatrix(location,location);
                for (int p=0; p<(int)combinationsOfRowsActionMatrix.size(); p++) {
                    float costInPeriod =0;
                    matrixInt_t transshipmentMatrix = initializeTransshipmentMatrix(location,location);
                    State *newState = new State(location);
                    newState->setState(*state);
                    for (int j=0; j<=location-1; j++) {
                        int sumTransshipAmountFromJ=0;
                        for (int k=0; k<=location-1; k++) {
                            int amountToTransship=0;
                            if(k!=j) amountToTransship=actionsForState[j][combinationsOfRowsActionMatrix[p][j]][k];
                            sumTransshipAmountFromJ+=amountToTransship;
                            newState->setElement(k,newState->getElement(k)+amountToTransship);
                            transshipmentMatrix[j][k]=amountToTransship;
                            
                        }
                        newState->setElement(j,newState->getElement(j)-sumTransshipAmountFromJ);
                    }
                    
                    costInPeriod-=getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                    //costInPeriod-=getHoldingCost(cost.getHoldingCost(), *newState);
                    costInPeriod+=solveByReactiveMethod(location,time, t, 1, 2, *newState, cost, distanceMatrix, distribution);
                    
                    
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState->setState(*newState);
                        for (int i=0; i<location; i++) {
                            for (int j=0; j<location; j++) {
                                maxTransshipmentMatrix[i][j]=transshipmentMatrix[i][j];
                            }
                        }
                    }
                    delete newState;
                    
                }
                
                
                state->setState(*maxState);
                delete maxState;
                
                totalProfit+=reward-holdingCost-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix);
                
                
            }
            else
                totalProfit+=reward-holdingCost;
            
        }
        delete state;
        profit[n]=totalProfit;
        
    }
    std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    //return (float)totalProfit/(float)numberOfIterations;
    return getStatistics(profit);
    
}

row_t solveByServiceLevelMethodProactive(const int& location, const int& time, const long& numberOfIterations, const State& initialState,const  Cost& cost, const Actions& distanceMatrix, map<int, pair<float, float> >& demandParameters)
{
    //map<int, pair<float, float> > probabilityParameters; // make a new type
    //probabilityParameters = initializeParameters("/Users/orusyaeva/Documents/ADP_Project/ADP/TransshipmentProject/TransshipmentProject/Probability_Input",location);
    row_t profit=row_t(numberOfIterations);
    
    vector<iDistribution*> distribution;
    distribution=initializeDemandDistribution(demandParameters,location,0);
    
    vector_t maxDemandForDistribution = getMaxDemandForDistribution(distribution,location);
    
    for (int n=0; n <numberOfIterations;n++)
    {
        //cout<<"---------n----------"<<n<<'\n';
        float totalProfit = 0;
        State* state = new State(location);
        state->setState(initialState);
        for (int t=0; t<time; t++) 
        {
            Demand *demand= new Demand(location);
            for (int i=0; i<location; i++) {
                demand->setElement(i, (int)distribution[i]->sample());
            }
                    
            float reward = getReward(cost.getSalesCost(),*state,*demand); 
            //cout<<"reward: "<<reward<<'\n';
            
            state->setState(getNewState(*state, *demand));//state after demand satisfaction
            
            float holdingCost=getHoldingCost(cost.getHoldingCost(), *state);

            delete demand;
            
            vector_t stock_out = getShortageLocations(*state,maxDemandForDistribution);
            vector_t left_over = getExcessLocations(*state,maxDemandForDistribution);
            
            if ((t!=time-1)&&((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) // for last time interval [T-1,T] there is no sence to make a transshipment
            {  
                
                list_t requiredQuantity = calculateRequiredQuantity(stock_out,*state,maxDemandForDistribution);
                list_t availableQuantity = calculateAvailableQuantity(left_over,*state,maxDemandForDistribution);
                
                //cout<<"required"<<'\n';
                //printList(requiredQuantity.size(),requiredQuantity);
                //cout<<"availble"<<'\n';
                //printList(availableQuantity.size(), availableQuantity);
                
                vecMatrixInt actionsForState=generateActionsForEachLocation(requiredQuantity,availableQuantity,location); //reduced 2 times
                
                /*for (int i=0; i<location; i++) {
                    printMatrix(actionsForState[i].size(), location, actionsForState[i]);
                    cout<<"----------------next----------------"<<'\n';
                }*/
                
                matrixInt_t combinationsOfRowsActionMatrix=generateAllCombinationsOfTransshipmentActions(actionsForState,location);
                
                //cout<<"size"<<(int)combinationsOfRowsActionMatrix.size()<<'\n';
                //for each possible action calulate cost
                float maxCost=-numeric_limits<float>::max();
                State *maxState = new State(location);
                matrixInt_t maxTransshipmentMatrix = initializeTransshipmentMatrix(location,location);
                for (int p=0; p<(int)combinationsOfRowsActionMatrix.size(); p++) {
                    float costInPeriod =0;
                    matrixInt_t transshipmentMatrix = initializeTransshipmentMatrix(location,location);
                    State *newState = new State(location);
                    newState->setState(*state);
                    for (int j=0; j<=location-1; j++) {
                        int sumTransshipAmountFromJ=0;
                        for (int k=0; k<=location-1; k++) {
                            int amountToTransship=0;
                            if(k!=j) amountToTransship=actionsForState[j][combinationsOfRowsActionMatrix[p][j]][k];
                            sumTransshipAmountFromJ+=amountToTransship;
                            newState->setElement(k,newState->getElement(k)+amountToTransship);
                            transshipmentMatrix[j][k]=amountToTransship;
                            
                        }
                        newState->setElement(j,newState->getElement(j)-sumTransshipAmountFromJ);
                        
                    }
                    State *minBorder = new State(location);
                    *minBorder=getDemandBorderForState(*newState,maxDemandForDistribution);
                    
                    matrixInt_t possibleDemands =generatePossibleDemands(*minBorder); 
                    //cout<<"size demand"<<possibleDemands.size()<<'\n';
                    delete minBorder;
                    float expectedTotalProfit=0;
                    
                    for (int i=0; i<=(int)(possibleDemands.size()-1); i++) 
                    {
                        Demand *demand= new Demand(location);
                        State *oldState = new State(location);
                        oldState->setState(*newState);
                        
                        float prob=1;
                        for (int j=0; j<location; j++) {
                            bool flag=true;
                            demand->setElement(j, possibleDemands[i][j]);
                            if ((oldState->getElement(j)-demand->getElement(j)==0)) {
                                flag=false;
                                prob*=probabilityOfDemand(demand->getElement(j),distribution[j],flag);
                            }
                            else
                                prob*=probabilityOfDemand(demand->getElement(j),distribution[j],flag);
                        }

                        float nextReward = getReward(cost.getSalesCost(),*oldState,*demand); 
                        
                        oldState->setState(getNewState(*oldState, *demand));
                        
                        float nextHoldingCost = getHoldingCost(cost.getHoldingCost(), *oldState);
                        
                        float error = getErrorCost(distribution, *oldState, distanceMatrix,cost.getTransshipmentCost());

                        expectedTotalProfit+=prob*(nextReward-nextHoldingCost-error);
                        
                        delete demand;
                        
                        delete oldState;

                    }
                    //cout<<"EX profit: "<<expectedTotalProfit<<'\n';
                    //cout<<"transshipment cost: "<<getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix)<<'\n';
                    costInPeriod=-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix)+expectedTotalProfit;
                    
                    if (costInPeriod>=maxCost) {
                        maxCost=costInPeriod;
                        maxState->setState(*newState);
                        for (int i=0; i<location; i++) {
                            for (int j=0; j<location; j++) {
                                maxTransshipmentMatrix[i][j]=transshipmentMatrix[i][j];
                            }
                        }
                    }
                    delete newState;
                    
                }
                state->setState(*maxState);
                delete maxState;
                //cout<<"transshipment Matrix"<<'\n';
                
                //printMatrix(location, location, maxTransshipmentMatrix);
                //cout<<"transshipment cost: "<<getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix)<<'\n';
                //cout<<"holding cost: "<<getHoldingCost(cost.getHoldingCost(), *state);
                totalProfit+=reward-holdingCost-getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, maxTransshipmentMatrix);
                
            }
            else
            {
                //cout<<"holding cost: "<<getHoldingCost(cost.getHoldingCost(), *state);
                totalProfit+=reward-holdingCost;
            }
            
        }
        delete state;
        profit[n]=totalProfit;
        
    }
    std::for_each( distribution.begin(), distribution.end(), boost::lambda::delete_ptr() );
    //return (float)totalProfit/(float)numberOfIterations;
    return getStatistics(profit);
    
}



    

