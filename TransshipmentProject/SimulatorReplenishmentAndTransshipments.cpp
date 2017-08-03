//
//  SimulatorReplenishmentAndTransshipments.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#include "SimulatorReplenishmentAndTransshipments.h"
#include "State.h"
#include "Profit.h"
#include "iProblem.h"
#include "Cost.h"
#include "Demand.h"
#include "ProblemMethods.h"
#include "iDistribution.h"
#include "Actions.h"
#include "LeadTimes.h"

void SimulatorReplenishmentAndTransshipments::simulate(iProblem& problem,Profit& profit)
{
    //get intial values
    int location = problem.getStateDimension();
    int time = problem.getTimeHorizon();
    
    Cost cost = problem.getCost();
    Demand demand=problem.getDemand();
    LeadTimes leadTimes=problem.getLeadTimes();
    
    matrixInt_t totalTransshipmentMatrix(location,vector_t(location,0));
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    State state(location,leadTimes.getMaxLeadTimePlusOne());

    
    //iterate from 0 to #iterations
    for (int n=0; n <iterations;n++)
    {
        //std::cout<<"-----------n: "<<n<<"------------"<<'\n';
        
        //std::cout<<"Initial state:"<<'\n';
        //State initialState = getInitialState(problem,n);
        State initialState=problem.getState();
        //initialState.print();
        
        //TODO change for the deterministic
        for (int i=0; i<location; i++) {
            profit.setOrderDecision(initialState.getElement(i,0),i);
        }
        
        int transshipmentSizePerIteration=0;
        int transshipmentDays=0;
        int stockoutEvents=0;
        
        float totalProfit = 0;
        
        totalProfit-=problemMethods::getCost(cost.getOrderingCost(),initialState);
        
        //std::cout<<"Ordering cost: "<<totalProfit<<'\n';
        
        //itialize the first state
        state=initialState;
        
        for (int t=0; t<time; t++)
        {
            //std::cout<<"time: "<<t<<'\n';
            int transshipmentSizePerPeriod=0;
            int transshipmentNumberPerPeriod=0;
            
            //std::cout<<"State in the begining of period "<<t<<'\n';
            //state.print();
            
            //observe demand
            //std::cout<<"Demand:"<<'\n';
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
            }
            //demand.print();
 
            //get reward
            float reward = problemMethods::getReward(cost.getSalesCost(),state,demand);
            
            for (int i=0; i<location; i++) {
                if (demand.getElement(i)>state.getElement(i,0)) {
                    stockoutEvents++;
                }
            }
            
            //get state after demand satisfaction
            problemMethods::getNewState(state, demand);
            
            //pay holding cost
            float holdingCost = problemMethods::getCost(cost.getHoldingCost(),state);
        
            totalProfit+=reward-holdingCost;
            
            float transshipmentCost=0;
            
            Actions maxTransshipmentMatrix(location);
            
            //get transshipment cost and next period state transshipments
            transshipmentCost=getTransshipmentCost(problem, t, state, n, maxTransshipmentMatrix);
            
            //std::cout<<"transshipment cost: "<<transshipmentCost<<'\n';
            
            for (int i=0; i<location; i++) {
                for (int j=0; j<location; j++) {
                    if (i!=j) {
                        if (maxTransshipmentMatrix.getElement(i,j)!=0) {
                            transshipmentSizePerPeriod+=maxTransshipmentMatrix.getElement(i,j);
                            transshipmentNumberPerPeriod++;
                            totalTransshipmentMatrix[i][j]++;
                        }
                    }
                }
            }
            
            if ((transshipmentCost!=0)&&(transshipmentNumberPerPeriod>0)) {
                transshipmentDays++;
            }
            //std::cout<<"Transshipments: "<<'\n';
            //maxTransshipmentMatrix.print();
            
            totalProfit-=transshipmentCost;

            profit.setTransshipmentSizePerPeriod(t,transshipmentSizePerPeriod+profit.getTransshipmentSizePerPeriod(t));
            profit.setFrequency(t,transshipmentNumberPerPeriod+profit.getFrequency(t));
            
            transshipmentSizePerIteration+=transshipmentSizePerPeriod;
            
            //std::cout<<"Period profit:"<<reward-holdingCost-transshipmentCost<<'\n';
            
            if (t==time-1) {
                for (int i=0; i<location; i++) {
                    for (int l=0; l<leadTimes.getMaxLeadTimePlusOne(); l++) {
                        profit.setLeftInventory(profit.getLeftInvetory(i,l)+state.getElement(i,l),i,l);
                    }
                }
            }
            
            
        }
        
        //std::cout<<"Profit: "<<totalProfit<<'\n';
        profit.setProfit(n,totalProfit);
        profit.setTransshipmentSizePerIteration(n,transshipmentSizePerIteration);
        profit.setTransshipmentDays(n,transshipmentDays);
        profit.setStockout(n,stockoutEvents);
        
    }
    profit.setTransshipmentMatrix(totalTransshipmentMatrix);
    
    
}

