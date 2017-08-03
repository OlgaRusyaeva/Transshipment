//
//  Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Simulator.h"
#include "State.h"
#include "Profit.h"
#include "iProblem.h"
#include "Cost.h"
#include "Demand.h"
#include "ProblemMethods.h"
#include "iDistribution.h"
#include "Actions.h"
#include "NumbersOperations.h"
#include "MyopicMethods.h"

using namespace myopicMethods;

void Simulator::simulate(iProblem& problem,Profit& profit)
{
    //get intial values
    int location = problem.getStateDimension();
    int time = problem.getTimeHorizon();
    int lifeTime= problem.getLifeTime();
    
    State initialState = problem.getState();
    Cost cost = problem.getCost();
    Demand demand=problem.getDemand();
    
    matrixInt_t totalTransshipmentMatrix = initializeTransshipmentMatrix(location,location);
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    State state(location,1,lifeTime);
    
    //std::cout<<"---------------"<<'\n';
    
    //iterate from 0 to #iterations
    for (int n=0; n <iterations;n++)
    {
        int transshipmentSizePerIteration=0;
        int transshipmentDays=0;
        
        //std::cout<<"n: "<<n<<'\n';
        float totalProfit = 0;
        
        //itialize the first state
        state=initialState;
        
        for (int t=0; t<time; t++) 
        {
            int transshipmentSizePerPeriod=0;
            int transshipmentNumberPerPeriod=0;
            
            
            //std::cout<<"time: "<<t<<'\n';
            
            //std::cout<<"State:"<<'\n';
            //state.print();

            
            float transshipmentCost=0;
        
            Actions maxTransshipmentMatrix(location);
            
            
            //get transshipment cost and next state after transshipments
            transshipmentCost=getTransshipmentCost(problem, t, state, n, distribution, maxTransshipmentMatrix);
            
            //std::cout<<"Transshipments:"<<'\n';
            //maxTransshipmentMatrix.print();
            
            //std::cout<<"Transshipment cost: "<<transshipmentCost<<'\n';
            
           		 //std::cout<<"Transshipments:"<<'\n';
           		 for (int i=0; i<location; i++) {
                    for (int j=0; j<location; j++) {
                    	if (i!=j) {
                        	if (maxTransshipmentMatrix.getElement(i,j)!=0) {
                            	transshipmentSizePerPeriod+=maxTransshipmentMatrix.getElement(i,j);
                            	transshipmentNumberPerPeriod++;
                            	totalTransshipmentMatrix[i][j]++;
                        	}
                    		//std::cout<<maxTransshipmentMatrix.getElement(i,j)<<" ";
                   	 	}
                    	else
                    	{
                        	//std::cout<<0<<" ";
                    	}
               	 	}
            	//std::cout<<'\n';
           		 }
            
            if ((transshipmentCost!=0)&&(transshipmentNumberPerPeriod>0)) {
                transshipmentDays++;
            }
            
            	totalProfit-=transshipmentCost;
                
                /*std::cout<<"State after:"<<'\n';
            	for (int i=0; i<location; i++) {
                	std::cout<<state.getElement(i)<<'\n';
            	}*/
            
            /*std::cout<<"State after action: "<<'\n';
            for (int i=0; i<state.getLength(); i++) {
                for (int l=0; l<state.getDepth(); l++) {
                    std::cout<<state.getElement(i,l)<<" ";
                }
                std::cout<<'\n';
            }*/
            
            //observe demand
            //std::cout<<"Demand:"<<'\n';
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
                //std::cout<<demand.getElement(i)<<'\n';
            }
            //demand.print();
            //get reward
            float reward = problemMethods::getReward(cost.getSalesCost(),state,demand); 
            
            //get state after demand satisfaction
            problemMethods::getNewState(state, demand);
            
            /*std::cout<<"State after demand: "<<'\n';
            for (int i=0; i<state.getLength(); i++) {
                for (int l=0; l<state.getDepth(); l++) {
                    std::cout<<state.getElement(i,l)<<" ";
                }
                std::cout<<'\n';
            }*/
            
            //pay holding cost
            float holdingCost = problemMethods::getCost(cost.getHoldingCost(),state);
            
            //add to the total profit
            totalProfit+=reward-holdingCost;
            
            //std::cout<<"Profit: "<<reward-holdingCost-transshipmentCost<<'\n';
            
            //no transshipments first time period
            profit.setTransshipmentSizePerPeriod(t,transshipmentSizePerPeriod+profit.getTransshipmentSizePerPeriod(t));
            profit.setFrequency(t,transshipmentNumberPerPeriod+profit.getFrequency(t));
            
            transshipmentSizePerIteration+=transshipmentSizePerPeriod;
            
            if (t==time-1) {
                for (int i=0; i<location; i++) {
                    for (int l=0; l<lifeTime; l++) {
                        profit.setLeftInventory(profit.getLeftInvetory(i,l)+state.getElement(i,l),i,l);
                    }
                }
            }
            
            problemMethods::getNewState(state);
        }        
        
        profit.setProfit(n,totalProfit);
        profit.setTransshipmentSizePerIteration(n,transshipmentSizePerIteration);
        profit.setTransshipmentDays(n,transshipmentDays);
        //transshipmentSizeTotal+=transshipmentSizePerIteration;
        //to change seed for each replication
		//problemMethods::resetSimulationParameters(distribution,seed,location,n);
        
    }
    profit.setTransshipmentMatrix(totalTransshipmentMatrix);
    
    
}



/*void Simulator::setResults(row_t& solution, Profit& profit)
{
	profit.setData(solution);
}*/
    

//get all Locations that out of stock after demand satisfaction
vector_t Simulator::getStockOutLocations(const State& initialState)
{
        const int L=initialState.getLength();
        vector_t stock_out;
        for (int i=0; i<=L-1; i++) 
        {
            if (initialState.getElement(i,0)==0)
                stock_out.push_back(i);
        }
        return stock_out;
        
}
    
    //initialize transshipment matrix by zero
matrixInt_t Simulator::initializeTransshipmentMatrix(const int& size_row, const int& size_col)
{
        return matrixInt_t(size_row ,vector_t(size_col,0));
        /*matrixInt_t Tr= matrixInt_t(size_row ,vector_t(size_col));
         for (int i=0; i<=size_row-1; i++) {
         for (int j=0; j<=size_col-1; j++) {
         Tr[i][j]= 0;
         }
         }
         return Tr;*/
        
}


//---------------------for test-------------------------//
void Simulator::printMatrix(const int& limit1, const int& limit2,const matrixInt_t& M)
{
    for (int i=0; i<= limit1-1; i++) {
        for (int j=0; j<=limit2-1; j++) {
            std::cout << std::setw(10)<< M[i][j];
        }
        std::cout << '\n';
    }
    
}



