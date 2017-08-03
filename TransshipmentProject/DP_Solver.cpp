//
//  DP_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "DP_Solver.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ProblemMethods.h"
#include "Output.h"
#include "NumbersOperations.h"


using namespace std;
using namespace arrays;

DP_Solver::~DP_Solver()
{
    delete valueFunction;
    delete hashFunction;
}
    
void DP_Solver::solve(iProblem& problem,const string& f_fileName)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    int LT = problem.getLifeTime();
        
    State initialState = problem.getState();
    Cost costInState = problem.getCost();
    Demand demand=problem.getDemand();

    getLookUpTable(L,T,LT, initialState, costInState, demand);

    //getLookUpTable(f_fileName);

        
    outputLookUpTable(f_fileName);
        
}

ValueFunction& DP_Solver::getValueFunction() const
{
    return *valueFunction;
}

Hashing& DP_Solver::getHashFunction() const
{
    return *hashFunction;
}


void DP_Solver::initializeValueFunction()
{
    valueFunction = new ValueFunction();
}

void DP_Solver::initializeHashFunction()
{
    hashFunction = new Hashing();
}
    

void DP_Solver::getLookUpTable(const int& location, const int& time, const int& lifeTime, const State& initialState,const Cost& cost, Demand& demand)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,0);
        
    State state(location,1,lifeTime);
    state=initialState;
        
    vector_t maxValue=getMaxValueForSumIL(state);

    int size_inventoryLevels=1;
    for (int i=0; i<location; i++) {
        for (int j=0; j<lifeTime; j++)
        size_inventoryLevels*=maxValue[j];
    }
    
    size_t initialStateHash=0;
        
    //for each time period
    for (int t=time-1; t>=0; t--) 
    {
            
        cout<<"time: "<<t<<'\n';
    
        int inv=0;
        //for each state in the state space
        //for (int inv=0; inv<allInventoryLevelsForInitialState.sizeH(); inv++)
        //{
        while (inv<size_inventoryLevels) {
            
            state=simulationMethods::generateInventoryLevel(maxValue.begin(),inv,location,lifeTime);
            
            bool feasibleState=true;
            //initialize state
            for (int i=0; i<lifeTime; i++) {
                int aggregateInv=0;
                for (int j=0; j<location; j++)
                {
                    aggregateInv+=state.getElement(j,i);
                    
                }
                if (aggregateInv>maxValue[i]) {
                    feasibleState=false;
                }
            }

                
            //find better solution
            //the sum inventories of possible state can't be more than the sum inventories of initial state
            if (feasibleState) {
                
                    
                initialStateHash = hashFunction->hash_value(state);
                
                
                /*for (int i=0; i<state.getLength(); i++) {
                    for (int l=0; l<state.getDepth(); l++) {
                        cout<<state.getElement(i,l)<<" ";
                    }
                    std::cout<<'\n';
                }*/
                
                float maxValue=getMaxProfit(t,time,state,demand,cost,distribution);
                valueFunction->setValueFunction(t,initialStateHash,maxValue);
                //cout<<"maxValue: "<<maxValue<<'\n';
                //cout<<"hash: "<<initialStateHash<<'\n';
                initialStateHash = 0;
            }

                    
        }
            
            
   	 } 
	//}
    //else
    //{
       // std::cout<<"The size of the state space is too big to calculate the value function by DP algorithm"<<'\n';
   // }
    
}



/*float DP_Solver::getMaxCostPost(const int& t, const State& state, const Cost& cost)const
{
    const int location=state.getLength();
    vecMatrixInt actionsForState=transhipmentFromState(state,false); 
    
    for (int i=0; i<location; i++) {
        std::cout<<"for location: "<<i<<'\n';
        for (int j=0; j<actionsForState[i].size(); j++) {
            for (int k=0; k<location; k++) {
                std::cout<<actionsForState[i][j][k]<<" ";
            }
            std::cout<<'\n';
        }
        std::cout<<"--------------------------"<<'\n';
    }
    
    State indexVector(location);
    int size_actions=1;
    simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
    
    Array2D<float> combinationsOfRowsActionMatrix(location, size_actions);
    combinationsOfRowsActionMatrix.initialize();
    simulationMethods::generatePossibleDemands(indexVector,combinationsOfRowsActionMatrix);
    
    for (int i=0; i<combinationsOfRowsActionMatrix.sizeH(); i++) {
        for (int j=0; j<combinationsOfRowsActionMatrix.sizeW(); j++) {
            std::cout<<combinationsOfRowsActionMatrix(i,j)<<" ";
        }
        std::cout<<'\n';
    }
    
    
    float maxCost=-numeric_limits<float>::max();
    
    State maxState(location);
    
    //row_t costInPeriods(location);
    //#pragma omp parallel for shared(costInPeriods,actionsForState,combinationsOfRowsActionMatrix)
    for (int p=0; p<combinationsOfRowsActionMatrix.sizeH(); p++) {
        float costInPeriod =0;
        State newState(location);
        Actions transshipmentMatrix(location);
        newState=state;
        for (int j=0; j<=location-1; j++) {
            int sumTransshipAmountFromJ=0;
            for (int k=0; k<=location-1; k++) {
                int amountToTransship=0;
                int index=(int)combinationsOfRowsActionMatrix(p,j);
                if(k!=j) amountToTransship=actionsForState[j][index][k];
                sumTransshipAmountFromJ+=amountToTransship;
                newState.setElement(k,newState.getElement(k)+amountToTransship);
                transshipmentMatrix.setElement(j,k,amountToTransship);
                
            }
            newState.setElement(j,newState.getElement(j)-sumTransshipAmountFromJ);
        }
        costInPeriod-=problemMethods::getTransshipmentCost(cost, transshipmentMatrix);
        
        costInPeriod+=valueFunction->getValueFunction(t+1, hashFunction->hash_value(newState));
        if (costInPeriod>=maxCost) {
            maxCost=costInPeriod;
            maxState=newState;
        }
        //costInPeriods[p]=costInPeriod;
        
    }
    return maxCost;
}


void DP_Solver::getLookUpTablePost(const int& location, const int& time, const State& initialState,const Cost& cost, Demand& demand)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,0);
    
    State state(location);
    state=initialState;
    
    int maxValue=getMaxValueForSumIL(state);
    State maxBorder(location);
    int size_inventoryLevels=1;
    getParametersForDemandGeneration(maxBorder,size_inventoryLevels,maxValue);
    
    Array2D<float> allInventoryLevelsForInitialState(location,size_inventoryLevels);
    allInventoryLevelsForInitialState.initialize();
    simulationMethods::generatePossibleDemands(maxBorder,allInventoryLevelsForInitialState);
    
    size_t initialStateHash=0;
    
    //test concavity
    //std::string fileName ("/Users/orusyaeva/Desktop/Calculations/Results.txt");
    //two dimensional case
    //matrix_t twoLocationProfit=matrix_t(maxValue+1,row_t(maxValue+1));
    //three dimensional case
    //vecMatrixFloat threeLocationProfit=vecMatrixFloat(maxValue+1,matrix_t(maxValue+1,row_t(maxValue+1)));

    for (int t=time-1; t>=0; t--) 
    {
        //output::cleanFile(fileName);

        
        //cout<<"time: "<<t<<'\n';
        for (int inv=0; inv<allInventoryLevelsForInitialState.sizeH(); inv++) 
        {
            float totalProfit = 0;
            int aggregateInv=0;
            
            for (int j=0; j<=location-1; j++) 
            {
                state.setElement(j,(int)allInventoryLevelsForInitialState(inv,j));
                aggregateInv+=state.getElement(j);
                
            }
            
            //find better solution
            if (aggregateInv<=maxValue) {
                
                cout<<"State:"<<'\n';
                for (int i=0; i<location; i++) {
                    cout<<state.getElement(i)<<'\n';
                }
                
                initialStateHash = hashFunction->hash_value(state);
                
                State minBorder(location);
                int size_demands=1;
                for (int i=0; i<location; i++) {
                    minBorder.setElement(i,state.getElement(i));
                    size_demands*=state.getElement(i)+1;
                }
                Array2D<float> possibleDemands(location,size_demands);
                possibleDemands.initialize();
                simulationMethods::generatePossibleDemands(minBorder,possibleDemands); 
                
                State stateAfterDemandSatisfaction(location);
                
                for (int i=0; i<possibleDemands.sizeH(); i++) 
                {
                    
                    float prob=1;
                    bool flag;
                    for (int j=0; j<location; j++) {
                        flag=true;
                        demand.setElement(j, (int)possibleDemands(i,j));
                        if ((state.getElement(j)-demand.getElement(j)==0)) {
                            flag=false;
                        }
                        prob*=problemMethods::probabilityOfDemand(demand.getElement(j),distribution[j],flag);
                    }
                    
                    
                    float reward = problemMethods::getReward(cost.getSalesCost(),state,demand); 
                    
                    stateAfterDemandSatisfaction=state;
                    
                    problemMethods::getNewState(stateAfterDemandSatisfaction, demand);
                    
                    float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
                    
                    if (t!=time-1) 
                    {
                        
                        //for (int p=0; p<(int)combinationsOfRowsActionMatrix.size(); p++) 
                         //{
                        // if (costInPeriods[p]>=maxCost) {
                        // maxCost=costInPeriods[p];
                        // }
                         
                        // }
                        
                        totalProfit+=prob*(reward-holdingCost+getMaxCostPost(t,stateAfterDemandSatisfaction,cost));
                        
                    }
                    else
                        totalProfit+=prob*(reward-holdingCost);
                    
                }
                //update value function for the state
                //cout<<"Value: "<<totalProfit<<'\n';
                valueFunction->setValueFunction(t,initialStateHash,totalProfit); 
                initialStateHash = 0;
            
            	//test concavity
             	//if (location==2){
                   // twoLocationProfit[state.getElement(0)][state.getElement(1)]=totalProfit;
                	//addToFile(fileName,state.getElement(0),state.getElement(1),totalProfit);
               // }
            
            	//if (location==3) {
                   // threeLocationProfit[state.getElement(0)][state.getElement(1)][state.getElement(2)]=totalProfit;
            	//}
                
            }
        }
        
        //test concavity
        //if (location==2){
            //createGraph(maxValue+1,maxValue+1,fileName,t);
			//checkConcavity(twoLocationProfit);
        //}
        
       // if (location==3) {
          //  checkConcavity(threeLocationProfit);
        //}
        //stop for each time period to check the file
        
    } 

    
}*/
   
void DP_Solver::addToFile(const std::string& fileName,const int& x, const int& y, const float& z)
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(),std::ios::app);
    
    outputFile<<x<<" "<<y<<" "<<z<<'\n';
    
    outputFile.close();
    
}

void DP_Solver::createGraph(const int& size1, const int& size2,const std::string& fileName, const int& time)
{
    
    output::outputValueFunction(size1,size2,fileName,time);

}

void DP_Solver::getLookUpTable(const string& f_fileName)
{
        ifstream file_input(f_fileName.c_str());
        valueFunction->inputVFunction(file_input);
        file_input.close();
}
    
    
void DP_Solver::outputLookUpTable(const string& f_fileName)
{
        ofstream file_output(f_fileName.c_str(),ios::out | ios::app);
        //ofstream file_output ("/Users/orusyaeva/Documents/ADP_Project/ADP/TransshipmentProject/TransshipmentProject/output");    
        valueFunction->printVFunction(file_output);
        file_output.close();
}


vector_t DP_Solver::getMaxValueForSumIL(const State& state)
{
    vector_t maxValue(state.getWidth());
    for (int j=0; j<state.getWidth(); j++) {
        int value=0;
        for (int i=0; i<state.getLength(); i++) {
            value+=state.getElement(i,j);
        }
        maxValue[j]=value;
    }
    return maxValue;
}

void DP_Solver::getParametersForDemandGeneration(State& maxBorder, int& size_inventoryLevels, const int& maxValue)
{
    for (int i=0; i<maxBorder.getLength(); i++) {
        maxBorder.setElement(i,0,maxValue);
        for (int t=0; t<maxBorder.getDepth(); t++) {
            size_inventoryLevels*=maxValue+1;
        }
    }
}

bool DP_Solver::checkConcavity(const matrix_t& matrix)
{
    bool flag=true;
    float diffR;
    float diffC;
    for (int i=0; i<matrix.size(); i++) {
        diffR=numeric_limits<float>::max();
        diffC=numeric_limits<float>::max();
        for (int j=0; j<matrix[i].size()-1; j++) {
            if(((numbersOperations::approximatelyEqual(diffR,matrix[i][j+1]-matrix[i][j],0.1))||(numbersOperations::definitelyGreaterThan(diffR,matrix[i][j+1]-matrix[i][j],0.1)))&&((numbersOperations::approximatelyEqual(diffC,matrix[j+1][i]-matrix[j][i],0.1))||(numbersOperations::definitelyGreaterThan(diffC,matrix[j+1][i]-matrix[j][i],0.1))))
            {
			//if (diffR>=(matrix[i][j+1]-matrix[i][j])&&(diffC>=matrix[j+1][i]-matrix[j][i])) {
            	diffR=matrix[i][j+1]-matrix[i][j];
                diffC=matrix[j+1][i]-matrix[j][i];
       	 	}
        	else
        	{
                if ((i+j)<matrix.size()-1) {
                    cout<<"Not concave on the domain"<<'\n';
                    cout<<"i: "<<i<<" and j: "<<j<<'\n';
                    cout<<"R: "<<(float)(diffR-matrix[i][j+1]+matrix[i][j])<<'\n';
                    cout<<"C: "<<(float)(diffC-matrix[j+1][i]+matrix[j][i])<<'\n';
                }
                else
                    cout<<"Not concave outside the domain"<<'\n';

           	 	flag=false;
           	 	break;
        	}
        }
        
    }
    
    return flag;
}

bool DP_Solver::checkConcavity(const vecMatrixFloat& matrix)
{
    bool flag=true;
    float diffR;
    float diffC;
    float diffH;
    for (int i=0; i<matrix.size(); i++) {
        for (int j=0; j<matrix[i].size(); j++) {
            diffR=numeric_limits<float>::max();
            diffC=numeric_limits<float>::max();
            diffH=numeric_limits<float>::max();
        	for (int k=0; k<matrix[i][j].size()-1; k++) {
            
            if((i+j+k)<matrix.size()-1)
            {
            	if(((numbersOperations::approximatelyEqual(diffR,matrix[i][j][k+1]-matrix[i][j][k],0.1))||(numbersOperations::definitelyGreaterThan(diffR,matrix[i][j][k+1]-matrix[i][j][k],0.1)))&&((numbersOperations::approximatelyEqual(diffC,matrix[k+1][i][j]-matrix[k][i][j],0.1))||(numbersOperations::definitelyGreaterThan(diffC,matrix[k+1][i][j]-matrix[k][i][j],0.1)))&&((numbersOperations::approximatelyEqual(diffH,matrix[i][k+1][j]-matrix[i][k][j],0.1))||(numbersOperations::definitelyGreaterThan(diffH,matrix[i][k+1][j]-matrix[i][k][j],0.1))))
            	{
				//if (diffR>=(matrix[i][j][k+1]-matrix[i][j][k])&&(diffC>=matrix[k+1][i][j]-matrix[k][i][j])&&(diffH>=matrix[i][k+1][j]-matrix[i][k][j])) 
            		diffR=matrix[i][j][k+1]-matrix[i][j][k];
               	 diffC=matrix[k+1][i][j]-matrix[k][i][j];
                	diffH=matrix[i][k+1][j]-matrix[i][k][j];
       	 		}
        		else
        		{
               	 if ((i+j+k)<matrix.size()) {
                 	   cout<<"Not concave on the domain"<<'\n';
                    	cout<<"i: "<<i<<" and j: "<<j<<" and k: "<<k<<'\n';
                    	cout<<"R: "<<(float)(diffR-matrix[i][j][k+1]+matrix[i][j][k])<<'\n';
                    	cout<<"C: "<<(float)(diffC-matrix[k+1][i][j]+matrix[k][i][j])<<'\n';
                    	cout<<"H: "<<(float)(diffH-matrix[i][k+1][j]+matrix[i][k][j])<<'\n';
                	}
                	else
                    	cout<<"Not concave outside the domain"<<'\n';
                
                flag=false;
                break;
        		}
            }
        }
        }
    }
    
    return flag;
}

    
