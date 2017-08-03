//
//  ADPCluster_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "ADPCluster_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "ValueFunction.h"
#include "Hashing.h"
#include "SimulationMethods.h"
#include "ProblemMethods.h"
#include "ActionsGenerator.h"
#include "Cluster.h"
#include "MyopicMethods.h"
#include "Demand.h"
#include "iProblem.h"

using namespace std;
using namespace arrays;

ADPCluster_Simulator::~ADPCluster_Simulator()
{
        for (int i=0; i<(int)VF.size(); i++) {
            delete VF[i];
        }
        
        for (int i=0; i<(int)HF.size(); i++) {
            delete HF[i];
        }
        
        delete CL;
}
    
void ADPCluster_Simulator::initializeValueFunction(const std::vector<ValueFunction>& valueFunction)
{
        for (int i=0; i<(int)valueFunction.size(); i++) {
            VF.push_back(new ValueFunction());
            *VF[i]=valueFunction[i];
        }
}
    
void ADPCluster_Simulator::initializeHashFunction(const std::vector<Hashing>& hashFunction)
{
        for (int i=0; i<(int)hashFunction.size(); i++) {
            HF.push_back(new Hashing());
            *HF[i]=hashFunction[i];
        }  
}
    
void ADPCluster_Simulator::initializeCluster(const Cluster& cluster)
{
        CL=new Cluster(cluster.getNumberOfClusters());
        *CL=cluster;
}
    
float ADPCluster_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{ 
  
    int time = problem.getTimeHorizon();
    Cost cost = problem.getCost();
    
    /*cout<<"State before transshipments:"<<'\n';
    for (int i=0; i<state.getLength(); i++) {
        cout<<state.getElement(i)<<'\n';
    }*/
    
    maxTransshipmentMatrix+=getActionsBetweenClusters(time, currentState,currentPeriod,cost,distribution);
    
    /*cout<<"Max transship matrix between clusters"<<'\n';
    for (int j=0; j<state.getLength(); j++) {
        for (int k=0; k<state.getLength(); k++) {
            cout<<maxTransshipmentMatrix.getElement(j,k)<<" ";
        }
        cout<<'\n';
    }
    
    cout<<"State after transshipments between clusters:"<<'\n';
    for (int j=0; j<state.getLength(); j++) {
        cout<<state.getElement(j)<<'\n';
    }*/
    
    for (int i=0; i<CL->getNumberOfClusters(); i++) {
        maxTransshipmentMatrix+=getActionsInsideCluster(time, currentState,currentPeriod,cost, i,distribution);
        
        /*cout<<"Max transship matrix for cluster "<<i<<'\n';
        for (int j=0; j<state.getLength(); j++) {
            for (int k=0; k<state.getLength(); k++) {
                cout<<maxTransshipmentMatrix.getElement(j,k)<<" ";
            }
            cout<<'\n';
        }
        
        cout<<"State after transshipments inside clusters:"<<'\n';
        for (int j=0; j<state.getLength(); j++) {
            cout<<state.getElement(j)<<'\n';
        }*/
        

            
    }
        
    return problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
        
}

Actions ADPCluster_Simulator::getActionsBetweenClusters(const int& time, State& state,const int& currentPeriod,const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    const int size=state.getLength();
    Actions bestTransshipment(size);
    
    State aggregatedState=getAggregatedState(state);
    const int aggSize=aggregatedState.getLength();
    
    //generate action space
    vecMatrixInt actionsForState=actions::transhipmentFromState(aggregatedState,false); 
    
    State indexVector(aggSize);
    int size_actions=1;
    simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
    arrays::Array2DInt<float> combinationsOfRowsActionMatrix(aggSize, size_actions);
    combinationsOfRowsActionMatrix.initialize(); 
    simulationMethods::generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix); 
    
    float bestProfit=-std::numeric_limits<float>::max();
    
    //generate demand sample
    Demand demand(size);
    arrays::Array2DInt<float> possibleDemands(size,demandNumber);
    possibleDemands.initialize();
    getDemandSample(possibleDemands,distribution);
    
    /*cout<<"Demand sample: "<<'\n';
    for (int i=0; i<possibleDemands.sizeH(); i++) {
        for (int j=0; j<possibleDemands.sizeW(); j++) {
            cout<<possibleDemands(i,j)<<" ";
        }
        cout<<'\n';
    }*/
    
    //for each action in the action space
    for (int p=0; p<combinationsOfRowsActionMatrix.sizeH(); p++) {
        
        float costPerAction =0;
        //state after making transshipments
        State postDecisionState(size);
        //current action
        Actions currentAggTransshipment(aggSize);
        Actions currentTransshipment(size);
        
        //initialize current action
        postDecisionState=state;
        for (int j=0; j<=aggSize-1; j++) {
            int sumTransshipAmountFromJ=0;
            
            for (int i=0; i<=aggSize-1; i++) {
                int amountToTransship=0;
                if(i!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][i];
                sumTransshipAmountFromJ+=amountToTransship;
                currentAggTransshipment.setElement(j,i,amountToTransship);
                
            }
        }
        
        currentTransshipment=transformActionsBetweenClusters(currentAggTransshipment,state, cost,distribution,time-currentPeriod);
        
        problemMethods::getNewState(postDecisionState, currentTransshipment);
        //get transshipment cost for doing action
        costPerAction-=problemMethods::getTransshipmentCost(cost, currentTransshipment);
        
        /*cout<<"Possible action:"<<'\n';
        for (int j=0; j<aggSize; j++) {
            for (int k=0; k<aggSize; k++) {
                cout<<currentAggTransshipment.getElement(j,k)<<" ";
            }
            cout<<'\n';
        }*/
        
        //for each demand in demand sample
        for (int i=0; i<possibleDemands.sizeH(); i++) 
        {
            //calculate probability
            float prob=(float)1/demandNumber;
            
            //cout<<"Probability: "<<prob<<'\n';
            
            //cout<<"Demand"<<'\n';
             for (int d=0; d<size; d++) {
                 demand.setElement(d,possibleDemands(i,d));
            	 //cout<<demand.getElement(d)<<'\n';
             }
            
            //get reward
            float reward = problemMethods::getReward(cost.getSalesCost(),postDecisionState,demand); 
            //cout<<"reward: "<<reward<<'\n';
            
            State stateAfterDemandSatisfaction(size);
            stateAfterDemandSatisfaction=postDecisionState;
            
            problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
            
            //get holding cost
            float holdingCost=problemMethods::getHoldingCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
            //cout<<"Holding cost:"<<holdingCost<<'\n';
            
            float valueNextPeriod=0;
            
            valueNextPeriod=VF[CL->getNumberOfClusters()]->getValueFunction(currentPeriod+1, HF[CL->getNumberOfClusters()]->hash_value(getAggregatedState(stateAfterDemandSatisfaction)));
                //cout<<"Time: "<<currentPeriod+1<<" and hash of state: "<<HF[CL->getNumberOfClusters()]->hash_value(stateAfterDemandSatisfaction)<<'\n';
            
            //cout<<"value next period: "<<valueNextPeriod<<'\n';
            //get value function
            
            costPerAction+=prob*(reward-holdingCost+valueNextPeriod);
            
        }
        
        if (costPerAction>=bestProfit) {
            bestProfit=costPerAction;
            //maxState=newState;
            bestTransshipment=currentTransshipment;
        }
        
    }
    
    //state after making transshipments
    problemMethods::getNewState(state, bestTransshipment);
    
    return bestTransshipment;

    
}


State ADPCluster_Simulator::getAggregatedState(const State& state)
{
    State aggregatedState(CL->getNumberOfClusters());
    for (int i=0; i<CL->getNumberOfClusters(); i++) {
        int IL=0;
        for (int j=0; j<CL->getNumberLocationsInCluster(i); j++) {
            IL+=state.getElement(CL->getLocationsForCluster(i)[j]);
        }
        aggregatedState.setElement(i,IL);
    }
    return aggregatedState;
    
}

Actions ADPCluster_Simulator::transformActionsBetweenClusters(const Actions& transshipment, const State& currentState, const Cost& cost,const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& restTime)
{
    State state=currentState;
    Actions transshipmentBetweenClusters(state.getLength());
    for (int i=0; i<CL->getNumberOfClusters(); i++) {
        for (int j=0; j<CL->getNumberOfClusters(); j++) {
            if (transshipment.getElement(i,j)>0) {
                makeTransshipmentsBetweenClusters(transshipmentBetweenClusters,CL->getLocationsForCluster(i),CL->getLocationsForCluster(j),transshipment.getElement(i,j),cost,distribution,state,restTime);
            }
        }
    }
        
    return transshipmentBetweenClusters;
}

void ADPCluster_Simulator::makeTransshipmentsBetweenClusters(Actions& transshipments, const vector_t& locationsInClusterA, const vector_t& locationsInClusterB, const int& maxAmountToTransship, const Cost& cost,const std::vector<boost::shared_ptr<iDistribution> >& distribution, State& state, const int& restTime)
{
    int amountToTransship=1;
	int location=state.getLength();
    //State newState(location);
    //newState=state;
    
    vector_t left_over;
    row_t profit_NoTransshipments(location);
    row_t profit_OneUnitMore(location);
    row_t profit_OneUnitLess(location);
    
    State stateOnePlus(location);
    State stateOneMinus(location);
    
    while (amountToTransship<=maxAmountToTransship) {
        
        left_over=myopicMethods::getNonStockOutLocations(state,locationsInClusterA);
        
        if ((int)left_over.size()!=0)
        { 
            
            myopicMethods::setOnePlusState(state,stateOnePlus);
            myopicMethods::setOneMinusState(state,stateOneMinus,left_over);
            
            
            profit_NoTransshipments=myopicMethods::getTransshipmentProfitRestPeriods(cost, distribution, state, restTime);
            profit_OneUnitMore=myopicMethods::getTransshipmentProfitRestPeriods(cost, distribution, stateOnePlus,restTime);
            profit_OneUnitLess=myopicMethods::getTransshipmentProfitRestPeriods(cost, distribution, stateOneMinus, restTime);
            
            int k=myopicMethods::getLowestDecreaseInProfit(profit_NoTransshipments,profit_OneUnitLess,left_over);
            int j=myopicMethods::getHighestIncreaseInProfit(profit_NoTransshipments,profit_OneUnitMore,locationsInClusterB);
            
            transshipments.setElement(k,j,transshipments.getElement(k,j)+1);
            state.setElement(k,state.getElement(k)-1);
            state.setElement(j,state.getElement(j)+1);
            
    	}
        amountToTransship++;
	}
}

void ADPCluster_Simulator::getDemandSample(arrays::Array2DInt<float>& possibleDemands, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    vector<boost::shared_ptr<iDistribution> > distributionVector;
    for (int k=0; k<possibleDemands.sizeW(); k++) {
        //cout<<"Mean for locaion: "<<k<<" is equal: "<<distribution[k]->mean()<<'\n';
        boost::shared_ptr<iDistribution> nd=problemMethods::makeConvolutionDistribution(distribution[k]->mean());
        distributionVector.push_back(nd);
    }
    
    for (int i=0; i<possibleDemands.sizeH(); i++) {
        for (int j=0; j<possibleDemands.sizeW(); j++) {
            possibleDemands(i,j)=distributionVector[j]->sample();//change here//use another distribution
        }
    }

    
}


Actions ADPCluster_Simulator::getActionsInsideCluster(const int& time, State& state,const int& currentPeriod,const Cost& cost, const int& clusterID,const vector<boost::shared_ptr<iDistribution> >& distribution)
{
    
        const int size=state.getLength();
        Actions bestTransshipment(size);
        
        State clusterState=getStateForCluster(state, clusterID);
        
        const int clusterSize=clusterState.getLength();
        
        vecMatrixInt actionsForState=actions::transhipmentFromState(clusterState,false); 
        
        State indexVector(clusterSize);
        int size_actions=1;
        simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
        arrays::Array2DInt<float> combinationsOfRowsActionMatrix(clusterSize, size_actions);
        combinationsOfRowsActionMatrix.initialize(); 
        simulationMethods::generatePossibleDemands(indexVector, combinationsOfRowsActionMatrix);              
        
        float bestProfit=-std::numeric_limits<float>::max();
        
        Demand demand(clusterSize);
        //State maxState(clusterSize);
        
        Actions bestTransshipmentInsideCluster(clusterSize);
        
        for (int p=0; p<combinationsOfRowsActionMatrix.sizeH(); p++) {
            
            float costPerAction =0;
            State postDecisionState(clusterSize);
            Actions currentTransshipment(clusterSize);
            
            postDecisionState=clusterState;
            for (int j=0; j<=clusterSize-1; j++) {
                int sumTransshipAmountFromJ=0;
                
                for (int i=0; i<=clusterSize-1; i++) {
                    int amountToTransship=0;
                    if(i!=j) amountToTransship=actionsForState[j][(int)combinationsOfRowsActionMatrix(p,j)][i];
                    postDecisionState.setElement(i,postDecisionState.getElement(i)+amountToTransship);
                    sumTransshipAmountFromJ+=amountToTransship;
                    currentTransshipment.setElement(j,i,amountToTransship);
                    
                }
                postDecisionState.setElement(j,postDecisionState.getElement(j)-sumTransshipAmountFromJ);
            }
            costPerAction-=problemMethods::getTransshipmentCost(cost, transformActionsInsideCluster(currentTransshipment,state, clusterID,cost));
            
            /*cout<<"Possible action:"<<'\n';
            for (int j=0; j<clusterSize; j++) {
                for (int k=0; k<clusterSize; k++) {
                    cout<<transshipmentMatrix.getElement(j,k)<<" ";
                }
                cout<<'\n';
            }*/
            
            //generate demand set
            int demandH=0;
            int demandW=clusterSize;
            getDemandSize(demandH,postDecisionState);
            Array2DInt<float> possibleDemands(demandW,demandH);
            possibleDemands.initialize();
            getDemandForCluster(postDecisionState,possibleDemands);
            
            
            //for each demand in demand set
            for (int i=0; i<possibleDemands.sizeH(); i++) 
            {
                //calculate probability
                float prob=1;
                bool flag;
                for (int j=0; j<clusterSize; j++) {
                    flag=true;
                    demand.setElement(j, (int)possibleDemands(i,j));
                    if ((postDecisionState.getElement(j)-demand.getElement(j))==0) {
                        flag=false;
                    }
                    prob*=problemMethods::probabilityOfDemand(demand.getElement(j),distribution[CL->getLocationsForCluster(clusterID)[j]],flag);
                }
                //sumProb+=prob;
                
                //cout<<"Probability: "<<prob<<'\n';
                
                /*cout<<"Demand"<<'\n';
                 for (int d=0; d<location; d++) {
                 cout<<demand.getElement(d)<<'\n';
                 }*/
                
                //get reward
                float reward = problemMethods::getReward(cost.getSalesCost(),transformClusterState(postDecisionState,size,clusterID),transformClusterDemand(demand,size,clusterID)); 
                //cout<<"reward: "<<reward<<'\n';
                
                State stateAfterDemandSatisfaction(clusterSize);
                stateAfterDemandSatisfaction=postDecisionState;
                
                problemMethods::getNewState(stateAfterDemandSatisfaction, demand);//!!!
                
                //get holding cost
                float holdingCost=problemMethods::getHoldingCost(cost.getHoldingCost(), transformClusterState(stateAfterDemandSatisfaction,size,clusterID));
                //cout<<"Holding cost:"<<holdingCost<<'\n';
                
                float valueNextPeriod=0;
                
                valueNextPeriod=VF[clusterID]->getValueFunction(currentPeriod+1, HF[clusterID]->hash_value(stateAfterDemandSatisfaction));
                
                //cout<<"value next period: "<<valueNextPeriod<<'\n';
                //get value function
                
                costPerAction+=prob*(reward-holdingCost+valueNextPeriod);
                
            }
            
            if (costPerAction>=bestProfit) {
                bestProfit=costPerAction;
                //maxState=newState;
                bestTransshipmentInsideCluster=currentTransshipment;
                
            }
            
        }
        
    /*cout<<"Max agg transship matrix:"<<'\n';
    for (int j=0; j<clusterSize; j++) {
        for (int k=0; k<clusterSize; k++) {
            cout<<maxAggtransshipmentMatrix.getElement(j,k)<<" ";
        }
        cout<<'\n';
    }*/
    
        bestTransshipment+=transformActionsInsideCluster(bestTransshipmentInsideCluster,state, clusterID,cost);
        
        problemMethods::getNewState(state, bestTransshipment);
        
        return bestTransshipment;
        
}



State ADPCluster_Simulator::getStateForCluster(const State& state, const int& clusterID)
{
    State newState(CL->getNumberLocationsInCluster(clusterID));
    for (int i=0; i<CL->getNumberLocationsInCluster(clusterID); i++) {
        newState.setElement(i,state.getElement(CL->getLocationsForCluster(clusterID)[i]));
    }
    return newState;
}
    

Actions ADPCluster_Simulator::transformActionsInsideCluster(const Actions& clusteredActions, const State& currentState, const int& clusterID, const Cost& cost)
{
    State state=currentState;
    Actions transshipmentInsideCluster(state.getLength());
    for (int i=0; i<CL->getNumberLocationsInCluster(clusterID); i++) {
        for (int j=0; j<CL->getNumberLocationsInCluster(clusterID); j++) {
            transshipmentInsideCluster.setElement(CL->getLocationsForCluster(clusterID)[i],CL->getLocationsForCluster(clusterID)[j],clusteredActions.getElement(i,j));
        }
    }
    return transshipmentInsideCluster;

}


void ADPCluster_Simulator::getDemandForCluster(const State& state,arrays::Array2DInt<float>& demandMatrix)
{
	State minBorder(state.getLength());
    for (int i=0; i<state.getLength(); i++) {
        minBorder.setElement(i,state.getElement(i));
    }
    simulationMethods::generatePossibleDemands(minBorder,demandMatrix); 

}
    
void ADPCluster_Simulator::getDemandSize(int& height, const State& state)
{
    int size_demands=1;
    for (int i=0; i<state.getLength(); i++) {
        size_demands*=state.getElement(i)+1;
    }
    height=size_demands;
}
    
State ADPCluster_Simulator::transformClusterState(const State& clusterState, const int& location, const int& clusterID)
{
    State state(location);
    for (int i=0; i<CL->getNumberLocationsInCluster(clusterID); i++) {
        state.setElement(CL->getLocationsForCluster(clusterID)[i],clusterState.getElement(i));
    }
    return state;
}

Demand ADPCluster_Simulator::transformClusterDemand(const Demand& clusterDemand, const int& location, const int& clusterID)
{
    Demand demand(location);
    for (int i=0; i<CL->getNumberLocationsInCluster(clusterID); i++) {
        demand.setElement(CL->getLocationsForCluster(clusterID)[i],clusterDemand.getElement(i));
    }
    return demand;
}


