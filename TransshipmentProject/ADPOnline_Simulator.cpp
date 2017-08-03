//
//  ADPOnline_Simulator.cpp
//  TransshipmentProject
//
//  Created by Olga on 21/03/16.
//
//

#include "ADPOnline_Simulator.hpp"
#include "LP_optimizer.h"
#include "iDistribution.h"
#include "State.h"
#include "Actions.h"
#include "Demand.h"
#include "Cost.h"
#include "ProblemMethods.h"
#include "iProblem.h"
#include "NumbersOperations.h"
#include "MyopicMethods.h"
#include "randomNumber.h"
#include <math.h>
#include "PLValueFunction.h"
#include "LeadTimes.h"
#include "SimulationMethods.h"
#include <stdlib.h>
#include <time.h>

ADPOnline_Simulator::ADPOnline_Simulator(const int& numberOfIterations, const int& seedNumber,iProblem& problem,const boost::shared_ptr<State>& initialState, const int& numberOfSamplePaths):SimulatorReplenishmentAndTransshipments(numberOfIterations,seedNumber)
{
    epsilon=0.3;
    step=20;
    sampleNumber=numberOfSamplePaths;
    initializeState(initialState);
    initializeValueFunction(problem);
}


void ADPOnline_Simulator::initializeValueFunction(iProblem& problem)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    LeadTimes leadTimes= problem.getLeadTimes();
    Cost cost = problem.getCost();
    int limit=leadTimes.getMaxLeadTimePlusOne();
    Demand demand=problem.getDemand();
    
    approxVF = boost::shared_ptr<PLValueFunction>(new PLValueFunction(T+1,L,limit));
    
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),L,20);
    
    initializeLastPeriodApproximation(T-1,cost,distribution);
    
    
}

void ADPOnline_Simulator::initializeState(const boost::shared_ptr<State>& upperBound)
{
    IL.reset(new State(*upperBound));
    
}

float ADPOnline_Simulator::getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, Actions& maxTransshipmentMatrix)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    LeadTimes leadTimes= problem.getLeadTimes();
    Cost cost = problem.getCost();
    int limit=leadTimes.getMaxLeadTimePlusOne();
    Demand demand=problem.getDemand();
    
    for (int n=0; n<sampleNumber; n++) {
        State sampleState=getSampleState(currentState, cost, currentPeriod,leadTimes);
        updateApproximation(L,T, cost,sampleState, demand, leadTimes,n,currentPeriod+1);
    }

    //approxVF->printBreakpoints();
    //approxVF->printSlopes();
    
    LP_optimizer lp_Problem;
    maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, approxVF,currentPeriod,leadTimes,limit,false);
    
    problemMethods::getNewState(currentState);
    
    problemMethods::getNewState(currentState, maxTransshipmentMatrix,leadTimes);
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost,maxTransshipmentMatrix);
    
    return transshipmentCost;
}

State ADPOnline_Simulator::getInitialState(iProblem& problem,const int& currentIteration)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    LeadTimes leadTimes= problem.getLeadTimes();
    Cost cost = problem.getCost();
    int limit=leadTimes.getMaxLeadTimePlusOne();
    Demand demand=problem.getDemand();
    State initialState(L,leadTimes.getMaxLeadTimePlusOne());
    
    for (int j=0; j<L; j++)
    {
        boost::shared_ptr<randomIntNumber> gn(new randomIntNumber(0,IL->getElement(j,0),j));
        randomILGenerator.push_back(gn);
    }
    
    for (int n=0; n<sampleNumber; n++) {
        if (currentIteration<=0.2*iterations) {
            initialState=getSampleInitialState(L, limit);
        }
        else
            initialState=getOrderDecision(L,limit,cost);

        updateApproximation(L,T, cost,initialState, demand, leadTimes,n,0);
    }
    
    
    return getOrderDecision(L, limit, cost);
    
}


void ADPOnline_Simulator::initializeLastPeriodApproximation(const int& time, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    
    const int L=IL->getLength();
    for (int i=0; i<L; i++) {
        int k=0;
        for (int j=0; j<=IL->getElement(i,0); j++) {
            k++;
            float lastPeriodProfitCurrentIL=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-myopicMethods::expectedValuePositivePart(1,distribution[i],j))-cost.getHoldingCost()[i]*j;
            float lastPeriodProfitILOnePlus=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-myopicMethods::expectedValuePositivePart(1,distribution[i],(j+1)))-cost.getHoldingCost()[i]*(j+1);
            float value=lastPeriodProfitILOnePlus - lastPeriodProfitCurrentIL;
            if (j>0) {
                approxVF->get(time,i,0).addBreakpoint(approxVF->get(time,i,0).getBreakpoint(j-1)+1);
                approxVF->get(time,i,0).addSlope(value);
            }
            if (j==0) {
                approxVF->get(time,i,0).setSlope(j,value);
            }
            
        }
        approxVF->get(time,i,0).setIndex(k);
    }
    
}

State ADPOnline_Simulator::getSampleState(const State& currentState, const Cost& cost, const int& currentPeriod,const LeadTimes& leadTimes)
{
    State sampleState=currentState;
    
    LP_optimizer lp_Problem;
    LP_solution decisions = lp_Problem.optimize(sampleState, cost, approxVF, currentPeriod,leadTimes,leadTimes.getMaxLeadTimePlusOne(),false);
    
    problemMethods::getNewState(sampleState);
    problemMethods::getNewState(sampleState, decisions,leadTimes);
    
    return sampleState;
}

void ADPOnline_Simulator::updateApproximation(const int& location, const int& time,const  Cost& cost,State& currentState, Demand& demand, const LeadTimes& leadTimes, const int& iterationNumber, const int& currentPeriod)
{
    	int maxLeadTime=leadTimes.getMaxLeadTimePlusOne();
        State post_decisionState(location,maxLeadTime);
        
        arrays::Array3D<float> gradientPlus(location,time-currentPeriod,maxLeadTime+1);
        arrays::Array3D<float> gradientMinus(location,time-currentPeriod,maxLeadTime+1);
        gradientPlus.initialize();
        gradientMinus.initialize();
        
        std::vector<State> statePath(time-currentPeriod,State(location,maxLeadTime));
        
        for (int t=currentPeriod; t<time-1; t++)
        {
           	post_decisionState=currentState;
            
            row_t profitWhatIfOneMore(location);
            row_t profitWhatIfOneLess(location);
            
            statePath[t-currentPeriod]=post_decisionState;
            
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
            }
            
            
            getProfitOnePlus(profitWhatIfOneMore,post_decisionState,demand, cost.getHoldingCost(), cost.getSalesCost());
            getProfitOneMinus(profitWhatIfOneLess,post_decisionState,demand, cost.getHoldingCost(), cost.getSalesCost());
            
            problemMethods::getNewState(currentState, demand);
            
            
            LP_optimizer lp_Problem;
            LP_solution decisions = lp_Problem.optimize(currentState, cost, approxVF, t,leadTimes,leadTimes.getMaxLeadTimePlusOne(),false);
            
            if (lp_Problem.checkIfOptimalBasicDegenerate())
            {
                lp_Problem.getTrueShadowPrices(decisions,location,leadTimes.getMaxLeadTimePlusOne(), leadTimes);
            }
            else
            {
                lp_Problem.getShadowPricesAsDualVariables(decisions);
                
            }
            
            
            for (int i=0; i<location; i++) {
                if (post_decisionState.getElement(i,0)<demand.getElement(i)) {
                    gradientPlus(t-currentPeriod,i,0)=profitWhatIfOneMore[i];
                    gradientMinus(t-currentPeriod,i,0)=profitWhatIfOneLess[i];
                }
                else
                {
                    if (post_decisionState.getElement(i,0)>demand.getElement(i)) {
                        gradientPlus(t-currentPeriod,i,0)=profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i);
                        gradientMinus(t-currentPeriod,i,0)=profitWhatIfOneLess[i]+decisions.getShadowPriceMinus(i);
                    }
                    else
                    {
                        gradientPlus(t-currentPeriod,i,0)=profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i);
                        gradientMinus(t-currentPeriod,i,0)=profitWhatIfOneLess[i];
                    }
                }
                
                for (int l=1; l<maxLeadTime+1; l++) {
                    gradientPlus(t-currentPeriod,i,l)=decisions.getShadowPricePlus(location+i*(maxLeadTime)+(l-1));
                    gradientMinus(t-currentPeriod,i,l)=decisions.getShadowPriceMinus(location+i*(maxLeadTime)+(l-1));
                }
                
            }
            
            problemMethods::getNewState(currentState);
            problemMethods::getNewState(currentState, decisions,leadTimes);
            
            
        }
        
        float updateGradientPlus=0;
        float updateGradientMinus=0;
        
        //std::cout<<"UPDATE:"<<'\n';
        for (int t=currentPeriod; t<time-2; t++) {
            //std::cout<<"Period: "<<t<<'\n';
            for (int i=0; i<location; i++) {
                // std::cout<<"Location: "<<i<<'\n';
                updateGradientPlus=gradientPlus(t-currentPeriod,i,0);
                updateGradientMinus=gradientMinus(t-currentPeriod,i,0);
                
                if (updateGradientMinus<updateGradientPlus) {
                    updateGradientMinus=updateGradientPlus;
                }
                
                approxVF->updateSlopeCave(t,i, 0,updateGradientPlus, updateGradientMinus, statePath[t-currentPeriod].getElement(i,0),location,iterationNumber,step);
                
                for (int l=1; l<maxLeadTime+1; l++) {
                    
                    //DUALMAX
                    //updateGradientPlus=getMaxGradient(gradientPlus,t,i,l,std::min(time-1-t,l));
            		//updateGradientMinus=getMinGradient(gradientMinus,t,i,l,std::min(time-1-t,l));
                    
                    //DUALNEXT
                    updateGradientPlus=gradientPlus(t+1-currentPeriod,i,l-1);
                    updateGradientMinus=gradientMinus(t+1-currentPeriod,i,l-1);
                    
                    if (updateGradientMinus<updateGradientPlus) {
                        updateGradientMinus=updateGradientPlus;
                    }
                    //std::cout<<"PLUS: "<<updateGradientPlus<<'\n';
                    //std::cout<<"MINUS: "<<updateGradientMinus<<'\n';
                    
                    approxVF->updateSlopeCave(t+1,i, l-1,updateGradientPlus, updateGradientMinus, statePath[t+1-currentPeriod].getElement(i,l-1),location,iterationNumber,step);
                    //printPLValueFunction();
                }
            }
        }
        
    
    
    
}




void ADPOnline_Simulator::getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();
    for (int i=0; i<L; i++) {
        if (state.getElement(i,0)<demand.getElement(i)) {
            profit[i]=salesPrice[i];
        }
        else
            profit[i]=-holdingCost[i];//-
    }
}

void ADPOnline_Simulator::getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
{
    const int L=state.getLength();
    for (int i=0; i<L; i++) {
        if (state.getElement(i,0)>demand.getElement(i)) {
            profit[i]=-holdingCost[i];//-
        }
        else
            profit[i]=salesPrice[i];
    }
    
    
}

float ADPOnline_Simulator::getMaxGradient(arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit)
{
    //std::cout<<"USE GRADIENTS OF FURTHER SUBPROBLEMS:"<<'\n';
    float max=-std::numeric_limits<float>::max();
    for (int s=1; s<=limit; s++) {
        //std::cout<<"time: "<<t+s<<" lead time: "<<l-s<<'\n';
        if (gradients(t+s,i,l-s)>max) {
            max=gradients(t+s,i,l-s);
        }
    }
    return max;
}

float ADPOnline_Simulator::getMinGradient(arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit)
{
    float min=std::numeric_limits<float>::max();
    for (int s=1; s<=limit; s++) {
        if (gradients(t+s,i,l-s)<min) {
            min=gradients(t+s,i,l-s);
        }
    }
    return min;
}

void ADPOnline_Simulator::printPLValueFunction()
{
    int T=approxVF->getSizeH();
    int L=approxVF->getSizeW();
    int maxLeadTime=approxVF->getSizeD();
    
    for (int t=0; t<T;t++){
        std::cout<<"Transshipment period: "<<t<<'\n';
        
        std::cout<<"Index vector:"<<'\n';
        for (int i=0; i<L; i++) {
            for (int l=0; l<maxLeadTime; l++) {
                std::cout<<"Index vector of location "<<i<<" lead time "<<l<<":"<<approxVF->get(t,i,l).getIndex()<<'\n';
            }
        }
        std::cout<<"Breakpoints:"<<'\n';
        for (int i=0; i<L; i++) {
            for (int l=0; l<maxLeadTime; l++) {
                std::cout<<"Breakpoints of location: "<<i<<" lead time "<<l<<":";
                for (int j=0; j<approxVF->get(t,i,l).getIndex(); j++) {
                    std::cout<<approxVF->get(t,i,l).getBreakpoint(j)<<" ";
                }
                std::cout<<'\n';
            }
        }
        std::cout<<"Slopes:"<<'\n';
        for (int i=0; i<L; i++) {
            for (int l=0; l<maxLeadTime; l++) {
                std::cout<<"SLopes of location: "<<i<<" lead time "<<l<<":";
                for (int j=0; j<approxVF->get(t,i,l).getIndex(); j++) {
                    std::cout<<approxVF->get(t,i,l).getSlope(j)<<" ";
                }
                std::cout<<'\n';
            }
        }
        
    }
}


State ADPOnline_Simulator::getOrderDecision(const int& location, const int& maxLeadTime, const Cost& cost)
{
    
    State sampleInitialState(location,maxLeadTime);

    for (int i=0; i<location; i++) {
    	sampleInitialState.setElement(i,0,approxVF->get(0,i,0).getOptimalBreakpoint(cost.getOrderingCost()[i]));
        for (int l=1; l<maxLeadTime; l++) {
            sampleInitialState.setElement(i,l,0);
        }
    }
    
    return sampleInitialState;
}

State ADPOnline_Simulator::getSampleInitialState(const int& location, const int& maxLeadTime)
{
    
    State sampleInitialState(location,maxLeadTime);

    for (int i=0; i<location; i++) {
        sampleInitialState.setElement(i,0,randomILGenerator[i]->sample());
        for (int l=1; l<maxLeadTime; l++) {
            sampleInitialState.setElement(i,l,0);
        }
    }

    return sampleInitialState;
}
