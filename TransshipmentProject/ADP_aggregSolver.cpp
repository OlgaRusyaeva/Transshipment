//
//  ADP_aggregSolver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/8/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ADP_aggregSolver.h"
#include "randomNumber.h"
#include "State.h"
#include "Actions.h"
#include "Demand.h"
#include "Cost.h"
#include "iDistribution.h"
#include "Hashing.h"
#include "ValueFunction.h"
#include "iProblem.h"
#include "Output.h"
#include "ADP_aggregSimulator.h"
#include "NumbersOperations.h"
#include "RangingMethods.h"
#include "Clustering.h"
#include "SimulationMethods.h"
#include "array.h"

using namespace std;
using namespace output;
using namespace numbersOperations;
    
void ADP_aggregSolver::solve(iProblem& problem, ADP_aggregSimulator& policy, const string& f_fileName)
{
        int L = problem.getNumberOfStates();
        int T = problem.getTimeHorizon();
        
        State initialState = problem.getState();
        Cost costInState = problem.getCost();
        Actions distance = problem.getDistances();
        Demand demand=problem.getDemand();
        
        //valueFunction.initializeClusters(L,distance);
        
        //initialState.setInitialState();
        //demand.setDemandParameters();
        
        getLookUpTable(L,T,initialState, costInState, distance, demand,policy.getValueFunction(),policy.getHashFunction());
        
        outputLookUpTable(policy.getValueFunction(),f_fileName);
        
        //getLookUpTable(policy.getValueFunction(),f_fileName);
        
}
    
    
    
void ADP_aggregSolver::getLookUpTable(const int& location, const int& time, const State& initialState,const Cost& cost, const Actions& distanceMatrix, Demand& demand, ValueFunction& valueFunction,Hashing& hashFunction)
{
        valueFunction.initializeClusters(location,distanceMatrix);
        for (int j=0; j<location; j++) {
            outputClusters(location,distanceMatrix,valueFunction.getCluster(j));
        }
        
        vector<boost::shared_ptr<iDistribution> > distribution;
        distribution=problemMethods::initializeDemandDistribution(demand.getDemandParameters(),location,20);
        
        vector_t maxDemandForDistribution = rangingMethods::getMaxDemandForDistribution(distribution,location);
        
        vector<size_t> hashVector=vector<size_t>(location);
        
        State state(location);
        
        for (int n=0; n <iterationNumber;n++)
        {

            for (int j=0; j<=location-1; j++) 
            {
                state.setElement(j,rand()%initialState.getElement(j)+1);
                
            }
            
            for (int t=0; t<time; t++) 
            {
                for (int l=0; l<location; l++) {
                    demand.setElement(l, (int)distribution[l]->sample());
                }
                
                for (int k=0; k<location; k++) {
                    hashVector[k]=hashFunction.hash_value(clustering::getClusteredState(state,k+1,valueFunction.getCluster(k)));
                }
                
                float reward = problemMethods::getReward(cost.getSalesCost(),state,demand); 
                
                state=problemMethods::getNewState(state, demand);//state after demand satisfaction
                
                float holdingCost=problemMethods::getHoldingCost(cost.getHoldingCost(), state);
                
                vector_t stock_out = rangingMethods::getShortageLocations(state,maxDemandForDistribution);
                vector_t left_over = rangingMethods::getExcessLocations(state,maxDemandForDistribution);
                
                if ((t!=time-1)&&((int)stock_out.size()!=0)&&((int)left_over.size()!=0)) // for last time interval [T-1,T] there is no sence to make a transshipment
                {  
                    
                    list_t requiredQuantity = rangingMethods::calculateRequiredQuantity(stock_out,state,maxDemandForDistribution);
                    list_t availableQuantity = rangingMethods::calculateAvailableQuantity(left_over,state,maxDemandForDistribution);
                    
                    double random =random_generator::randomDoubleNumber(0, 1);
                    
                    if (random <= epsilon) 
                    {
                        Actions transshipmentMatrix2(location);
                        while (((int)requiredQuantity.size()!=0)&&((int)availableQuantity.size()!=0)) {
                            
                            requiredQuantity.sort(rangingMethods::MyDataSortPredicateDescending);
                            
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
                        
                        vecMatrixInt actionsForState=clustering::generateActionsForEachLocation(requiredQuantity,availableQuantity,location);   
                        
                        State indexVector(location);
                        int size_actions=1;
                        simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
                        arrays::Array2DInt<float> combinationsOfRowsActionMatrix(location, size_actions);
                        combinationsOfRowsActionMatrix.initialize();    
                        simulationMethods::generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);
                        
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
                            costInPeriod-=problemMethods::getTransshipmentCost(cost.getTransshipmentCost(), distanceMatrix, transshipmentMatrix);
                            
                            float costOfValueFunction=0;
                            for (int i=0; i<location; i++) {
                                costOfValueFunction+=((i+1)/location)*valueFunction.getValueFunction(t+1, hashFunction.hash_value(clustering::getClusteredState(newState,i+1,valueFunction.getCluster(i))),i);
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
    
void ADP_aggregSolver::getLookUpTable(ValueFunction& valueFunction, const string& f_fileName)
{
    ifstream file_input(f_fileName.c_str());
    valueFunction.inputVFunction(file_input);
    file_input.close();
}
    
    
void ADP_aggregSolver::outputLookUpTable(ValueFunction& valueFunction,const string& f_fileName)
{
        ofstream file_output(f_fileName.c_str());
        //ofstream file_output ("/Users/orusyaeva/Documents/ADP_Project/ADP/TransshipmentProject/TransshipmentProject/output");    
        valueFunction.printVFunction(file_output);
        file_output.close();
}
    
void ADP_aggregSolver::outputClusters(const int&location,const Actions& distance,const vector_t& clusters)
{
    outputClustersMap(location,distance,clusters);        
}

void ADP_aggregSolver::sortAvailableQuantity(list_t& availableQuantity,row_t& distanceToFirstShortageElement)
{
    int j=availableQuantity.front().first;
    float quantity=availableQuantity.front().second;
    float distance=distanceToFirstShortageElement[j];
    
    for (list_t::const_iterator i=availableQuantity.begin(); i!=availableQuantity.end(); i++) {
        if (distanceToFirstShortageElement[(*i).first]<distance) {
            j=(*i).first;
            quantity=(*i).second;
            distance=distanceToFirstShortageElement[j];
        }
    }
    availableQuantity.remove(make_pair(j, quantity));
    availableQuantity.push_front(make_pair(j, quantity));
}
