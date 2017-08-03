//
//  ADPMultipleTransshipments_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 21/06/16.
//
//

#include "ADPMultipleTransshipments_Solver.hpp"
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


void ADPMultipleTransshipments_Solver::getApproximation(const int& location, const int& time, const int& lifeTime, State& initialState,const  Cost& cost,  Demand& demand, const LeadTimes& leadTimes, const int& iterationNumber)
{
    State state(location);
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,20);
    
    randomDoubleNumber randomNumberGenerator(0,1,iterationNumber);
    vector<boost::shared_ptr<randomIntNumber> > randomILGenerator(location);
    for (int j=0; j<location; j++)
    {
        boost::shared_ptr<randomIntNumber> gn(new randomIntNumber(0,initialState.getElement(j,0),j));
        randomILGenerator[j]=gn;
    }
    
    
    //last period initialization
    initializeLastPeriodApproximation(time-1,initialState,cost,distribution);
    
    for (int n=0; n <iterationNumber;n++)
    {
        
        //std::cout<<"n: "<<n<<'\n';
        //printPLValueFunction();
        
        state=initialState;
        
        State post_decisionState(location);
        
        for (int t=0; t<time; t++)
        {
            //std::cout<<"t: "<<t<<'\n';
            LP_optimizer lp_Problem;
            LP_solution decisions = lp_Problem.optimize(state, 0,cost, (*approxVF).getPiecewiseLinearFunctionFor(0), t);
            
            if (t!=0) {
                if (lp_Problem.checkIfOptimalBasicDegenerate())
                {
                    lp_Problem.getTrueShadowPrices(decisions,location,1, leadTimes);
                }
                else
                {
                    lp_Problem.getShadowPricesAsDualVariables(decisions);
                    
                }
                
                row_t profitWhatIfOneMore(location);
                row_t profitWhatIfOneLess(location);
                
                getProfitOnePlus(profitWhatIfOneMore,post_decisionState,1,demand, cost.getHoldingCost(), cost.getSalesCost());
                getProfitOneMinus(profitWhatIfOneLess,post_decisionState,1,demand, cost.getHoldingCost(), cost.getSalesCost());
                
                for (int i=0; i<location; i++) {
                    
                    if (post_decisionState.getElement(i,0)<demand.getElement(i)) {
                        //std::cout<<"1"<<'\n';
                        //std::cout<<profitWhatIfOneMore[i]<<'\n';
                        //std::cout<<profitWhatIfOneLess[i]<<'\n';
                        approxVF->updateSlopeLeveling(t-1,i, 0,profitWhatIfOneMore[i], profitWhatIfOneLess[i], post_decisionState.getElement(i,0),n);
                    }
                    else
                    {
                        if (post_decisionState.getElement(i,0)>demand.getElement(i)) {
                            //std::cout<<"2"<<'\n';
                            //std::cout<<profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i)<<'\n';
                            //std::cout<<profitWhatIfOneLess[i]+decisions.getShadowPriceMinus(i)<<'\n';
                            approxVF->updateSlopeLeveling(t-1,i, 0,profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i), (profitWhatIfOneLess[i]+decisions.getShadowPriceMinus(i)), post_decisionState.getElement(i,0),n);
                        }
                        else
                        {
                            //std::cout<<"3"<<'\n';
                            //std::cout<<profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i)<<'\n';
                            //std::cout<<profitWhatIfOneLess[i]<<'\n';
                            approxVF->updateSlopeLeveling(t-1,i, 0, profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i), profitWhatIfOneLess[i], post_decisionState.getElement(i,0),n);
                        }
                    }
                    
                    
                }
            }
            
            double random =randomNumberGenerator.sample();
            double harmonic_epsilon=(1/pow(n+1,epsilon));
            
            if (random<=harmonic_epsilon) {
                
                for (int j=0; j<=location-1; j++)
                {
                    state.setElement(j,0,randomILGenerator[j]->sample());
                }
            }
            else
            {
                problemMethods::getNewState(state, decisions);
                
            }
            
            post_decisionState=state;
            
            
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
            }
            
            problemMethods::getNewState(state, demand);
            
        }
        
    }
    
}