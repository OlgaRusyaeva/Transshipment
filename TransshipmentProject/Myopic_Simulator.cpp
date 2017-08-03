//
//  Myopic_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/24/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Myopic_Simulator.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "MyopicMethods.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "LeadTimes.h"

using namespace std;
    
 float Myopic_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix)
{

    int location = problem.getStateDimension();
    int time = problem.getTimeHorizon();
    Cost cost = problem.getCost();
    LeadTimes leadTimes =problem.getLeadTimes();
    
        bool do_Transshipment=true;
    
        vector_t left_over;
    	vector_t sourceVector=myopicMethods::getLocationsForTransshipments(currentState);
    
        row_t profit_NoTransshipments(location);
        row_t profit_OneUnitMore(location);
        row_t profit_OneUnitLess(location);
    
        State stateOnePlus(location);
        State stateOneMinus(location);
        
        while (do_Transshipment) 
        {
            left_over=myopicMethods::getNonStockOutLocations(currentState,sourceVector);
            
            if ((int)left_over.size()!=0)
            { 
                /*cout<<"Left overs:"<<'\n';
                for (int i=0; i<(int)left_over.size(); i++) {
                    cout<<left_over[i]<<'\n';
                }*/
                
                myopicMethods::setOnePlusState(currentState,stateOnePlus);
                myopicMethods::setOneMinusState(currentState,stateOneMinus,left_over);
                
                /*cout<<"stateOnePlus:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<stateOnePlus.getElement(i)<<'\n';
                }
                
                cout<<"stateOneMinus:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<stateOneMinus.getElement(i)<<'\n';
                }*/
                profit_NoTransshipments=myopicMethods::getTransshipmentProfitRestPeriods(cost, distribution, currentState, time-currentPeriod);
                profit_OneUnitMore=myopicMethods::getTransshipmentProfitRestPeriods(cost, distribution, stateOnePlus,time-currentPeriod);
                profit_OneUnitLess=myopicMethods::getTransshipmentProfitRestPeriods(cost, distribution, stateOneMinus, time-currentPeriod);

                
                /*cout<<"profit_NoTransshipments:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<profit_NoTransshipments[i]<<'\n';
                }
              
                
                cout<<"profit_OneUnitMore:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<profit_OneUnitMore[i]<<'\n';
                }
                
                cout<<"profit_OneUnitLess:"<<'\n';
                for (int i=0; i<state.getLength(); i++) {
                    cout<<profit_OneUnitLess[i]<<'\n';
                }*/
                
                
                int k=myopicMethods::getLowestDecreaseInProfit(profit_NoTransshipments,profit_OneUnitLess,left_over);
                int j=myopicMethods::getHighestIncreaseInProfit(profit_NoTransshipments,profit_OneUnitMore,sourceVector);
                
                //cout<<"k: "<<k<<'\n';
                //cout<<"j: "<<j<<'\n';

                
                if ((k!=j)&&(profit_OneUnitMore[j]-profit_NoTransshipments[j])-(profit_NoTransshipments[k]-profit_OneUnitLess[k])>cost.getTransshipmentCost()[k]*cost.getDistance(k,j))
                {
                    maxTransshipmentMatrix.setElement(k,j,maxTransshipmentMatrix.getElement(k,j)+1);
                    currentState.setElement(k,0, currentState.getElement(k,0)-1);
                    currentState.setElement(j,0, currentState.getElement(j,0)+1);
                }
                else
                    do_Transshipment=false;
                
            }
            else
                do_Transshipment=false;
        }
        
        
    	float transshipmentCost = problemMethods::getTransshipmentCost(cost, maxTransshipmentMatrix);
        
        return transshipmentCost;
        
}



