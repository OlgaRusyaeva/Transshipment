//
//  Simulator_Pre.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/25/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "Simulator_Pre.h"
#include "State.h"
#include "Cost.h"
#include "Demand.h"
#include "ProblemMethods.h"
#include "iDistribution.h"
#include "Actions.h"
#include "Profit.h"


void Simulator_Pre::simulator(const int& location, const int& time, const State& initialState,const Cost& cost, Demand& demand,  Profit& profit, const int& numberOfIterations)
{

    int transshipmentSizeTotal=0;
    matrixInt_t totalTransshipmentMatrix = initializeTransshipmentMatrix(location,location);
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    State state(location);
    
    for (int n=0; n <numberOfIterations;n++)
    {
        int transshipmentSizePerIteration=0;
        
        std::cout<<"n: "<<n<<'\n';
        float totalProfit = 0;
        
        state=initialState;
        
        for (int t=0; t<time; t++) 
        {
            int transshipmentSizePerPeriod=0;
            int transshipmentNumberPerPeriod=0;
            
            std::cout<<"time: "<<t<<'\n';
            
            std::cout<<"State:"<<'\n';
            for (int i=0; i<location; i++) {
                std::cout<<state.getElement(i)<<'\n';
            }
            Actions maxTransshipmentMatrix(location);
            
            float transshipmentCost=0;
            
            if (t!=0) {
                transshipmentCost=getTransshipmentCost(location, time, t, state, cost, distribution, maxTransshipmentMatrix);
            }
            
            std::cout<<"Transshipments:"<<'\n';
            for (int i=0; i<location; i++) {
                for (int j=0; j<location; j++) {
                    if (maxTransshipmentMatrix.getElement(i,j)!=0) {
                        transshipmentSizePerPeriod+=maxTransshipmentMatrix.getElement(i,j);
                        transshipmentNumberPerPeriod++;
                        totalTransshipmentMatrix[i][j]++;
                    }
                    std::cout<<maxTransshipmentMatrix.getElement(i,j)<<" ";
                }
                std::cout<<'\n';
            }
            
            totalProfit-=transshipmentCost;
            
            std::cout<<"Demand:"<<'\n';
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
                std::cout<<demand.getElement(i)<<'\n';
            }
            
            float reward = problemMethods::getReward(cost.getSalesCost(),state,demand); 
            
            problemMethods::getNewState(state, demand);
            
            float holdingCost = problemMethods::getHoldingCost(cost.getHoldingCost(),state);
            
            totalProfit+=reward-holdingCost;
            
            std::cout<<"Profit: "<<reward-holdingCost-transshipmentCost<<'\n'; 
            
            //no transshipments first time period
            profit.setTransshipmentSizePerPeriod(t,transshipmentSizePerPeriod+profit.getTransshipmentSizePerPeriod(t));
            profit.setFrequency(t,transshipmentNumberPerPeriod+profit.getFrequency(t));
            
            transshipmentSizePerIteration+=transshipmentSizePerPeriod;
            
            
        }        
        
        profit.setProfit(n,totalProfit);
        profit.setTransshipmentSizePerIteration(n,transshipmentSizePerIteration);
        transshipmentSizeTotal+=transshipmentSizePerIteration;
        //to change seed for each replication
		//problemMethods::resetSimulationParameters(distribution,seed,location,n);
        
    }
    profit.setTransshipmentMatrix(totalTransshipmentMatrix);
    
    
}
