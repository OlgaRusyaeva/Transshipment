//
//  ADPLifeTimeTransshipment_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 21/06/16.
//
//

#include "ADPLifeTimeTransshipment_Solver.hpp"
#include "iDistribution.h"
#include "randomNumber.h"
#include "State.h"
#include "Actions.h"
#include "Demand.h"
#include "Cost.h"
#include "LeadTimes.h"
#include "LP_optimizer.h"
#include "ProblemMethods.h"
#include "PLValueFunction.h"

void ADPLifeTimeTransshipment_Solver::getApproximation(const int& location, const int& time, const int& lifeTime, State& initialState,const  Cost& cost,Demand& demand, const LeadTimes& leadTimes, const int& iterationNumber)
{
    State state(location,1,lifeTime);
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,20);
    
    randomDoubleNumber randomNumberGenerator(0,1,iterationNumber);
    vector<boost::shared_ptr<randomIntNumber> > randomILGenerator(location);
    for (int j=0; j<location; j++)
    {
        int sumInventory=0;
        for (int m=0; m<lifeTime; m++) {
            sumInventory+=initialState.getElement(j,m);
        }
        boost::shared_ptr<randomIntNumber> gn(new randomIntNumber(0,sumInventory,j));
        randomILGenerator[j]=gn;

    }
    
    
    //initialize last period value function for m=0
    initializeLastPeriodApproximation(time-1,initialState,cost,distribution);
    
    //for each iteration
    for (int n=0; n<iterationNumber; n++) {
        //take intial state
        state=initialState;
        
        //intialize post_decision state
        State post_decisionState(location,1,lifeTime);
        
        //for each time period (from 0 till time (incl))
        for (int t=0; t<=time; t++) {
            
            std::vector<LP_solution> decisionsForEachLifeTime(lifeTime,LP_solution(location,2));
            for (int m=0; m<lifeTime; m++) {
                //solve LP problem for each m
                LP_optimizer lp_Problem;
                decisionsForEachLifeTime[m] = lp_Problem.optimize(state, m,cost, (*approxVF).getPiecewiseLinearFunctionFor(m), t);
                
                //check degenerance for each m
                if (t!=0) {
                    //get dual values for each m
                    if (lp_Problem.checkIfOptimalBasicDegenerate())
                    {
                        lp_Problem.getTrueShadowPrices(decisionsForEachLifeTime[m],location,1, leadTimes);
                    }
                    else
                    {
                        lp_Problem.getShadowPricesAsDualVariables(decisionsForEachLifeTime[m]);
                        
                    }
                    
                    //calculate marginal value +1/-1 for each m
                    //row_t profitWhatIfOneMore(location);
                    //row_t profitWhatIfOneLess(location);
                    
                    //getProfitOnePlus(profitWhatIfOneMore,post_decisionState,m,demand, cost.getHoldingCost(), cost.getSalesCost());
                    //getProfitOneMinus(profitWhatIfOneLess,post_decisionState,m,demand, cost.getHoldingCost(), cost.getSalesCost());
                }
            }
                    
            //update slopes  of the value function at time t-1 looking at previous period demand and current post_decison state for each m
            
            arrays::Array2D<float> gradientPlus(location,lifeTime);
            arrays::Array2D<float> gradientMinus(location,lifeTime);
            gradientPlus.initialize();
            gradientMinus.initialize();
            
            if (t!=0) {
                for (int i=0; i<location; i++) {
                    for (int m=lifeTime-1; m>=0; m--) {
                        //calculate pi
                        int sumInventory=0;
                        for (int s=0; s<m; s++) {
                            sumInventory+=post_decisionState.getElement(i,m);
                        }
                        
                        if (sumInventory<demand.getElement(i)) {
                            if (m==lifeTime-1) {
                                gradientPlus(m,i)=cost.getSalesCost()[i]+decisionsForEachLifeTime[m-1].getShadowPricePlus(i);
                                gradientMinus(m,i)=cost.getSalesCost()[i]+decisionsForEachLifeTime[m-1].getShadowPriceMinus(i);
                            }
                            else
                            {
                                gradientPlus(m,i)=gradientPlus(m+1,i);
                                gradientMinus(m,i)=gradientMinus(m+1,i);
                            }
                        }
                        else
                        {
                            if (sumInventory>demand.getElement(i)) {
                                gradientPlus(m,i)=-cost.getHoldingCost()[i]+decisionsForEachLifeTime[m-1].getShadowPricePlus(i);
                                gradientMinus(m,i)=-cost.getHoldingCost()[i]+decisionsForEachLifeTime[m-1].getShadowPriceMinus(i);
                            }
                            else
                            {
                                gradientPlus(m,i)=-cost.getHoldingCost()[i]+decisionsForEachLifeTime[m-1].getShadowPricePlus(i);
                                if (m==lifeTime-1) {
                                    gradientMinus(m,i)=cost.getSalesCost()[i]+decisionsForEachLifeTime[m-1].getShadowPriceMinus(i);
                                }
                                else
                                    gradientMinus(m,i)=gradientMinus(m+1,i);
                            }
                        }
                        
                        
                        //update value function
                        approxVF->updateSlopeCave(t-1,i, m,gradientPlus(m,i), gradientMinus(m,i), post_decisionState.getElement(i,m),location,n,step);
                    }
                }
            }


            
            //explotation/exploration next state
            double random =randomNumberGenerator.sample();
            double harmonic_epsilon=(1/pow(n+1,epsilon));
            
            if (random<=harmonic_epsilon) {
                
                for (int j=0; j<=location-1; j++)
                {
                    for (int m=0; m<lifeTime; m++) {
                    state.setElement(j,m,randomILGenerator[j]->sample());
                    }

                }
            }
            else
            {
                for (int m=0; m<lifeTime; m++) {
                    problemMethods::getNewState(state, decisionsForEachLifeTime[m],m);
                }

                
            }
            
            //observe next period demand
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
            }
            
            problemMethods::getNewState(state, demand);
            
            problemMethods::getNewState(state);

        }

    }
    

    
}