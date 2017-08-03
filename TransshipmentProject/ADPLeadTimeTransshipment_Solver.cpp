//
//  ADPLeadTimeTransshipment_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#include "ADPLeadTimeTransshipment_Solver.h"
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
#include <boost/thread.hpp>
#include <boost/thread/detail/thread_group.hpp>

using namespace numbersOperations;

boost::shared_ptr<PLValueFunction> ADPLeadTimeTransshipment_Solver::getPiecewiseApp_ValueFunction() const
{
    return approxVF;
}

boost::shared_ptr<State> ADPLeadTimeTransshipment_Solver::getInitialState() const
{
    return IL;
}

void ADPLeadTimeTransshipment_Solver::initializePiecewiseApp_ValueFunction(const int& location, const int& time, const int& maxLeadTime)
{
    approxVF = boost::shared_ptr<PLValueFunction>(new PLValueFunction(time,location,maxLeadTime));
}

void ADPLeadTimeTransshipment_Solver::initializeState(const boost::shared_ptr<State>& upperBound)
{
    IL.reset(new State(*upperBound));
}

void ADPLeadTimeTransshipment_Solver::solveAsSinglePeriod(iProblem& problem, const int& iterationNumber)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    LeadTimes leadTimes= problem.getLeadTimes();
    for (int i=0; i<L; i++) {
        IL->setElement(i,0,problem.getState().getElement(i,0));
    }
    
    //T+1 because of LP otimizer, value function at T+1 is always zero
    initializePiecewiseApp_ValueFunction(L,T+1,leadTimes.getMaxLeadTimePlusOne());
    
    
    Cost costInState = problem.getCost();
    Demand demand=problem.getDemand();
    
    getApproximationSinglePeriodUpdate(L,T, costInState, demand, iterationNumber,leadTimes);
    
    printPLValueFunction();
    
	setInitialState(costInState);
    
    
}

void ADPLeadTimeTransshipment_Solver::solveAsMultiPeriod(iProblem& problem, const int& iterationNumber)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    LeadTimes leadTimes= problem.getLeadTimes();
    for (int i=0; i<L; i++) {
        IL->setElement(i,0,problem.getState().getElement(i,0));
    }
    //T+1 because of LP otimizer, value function at T+1 is always zero
    initializePiecewiseApp_ValueFunction(L,T+1,leadTimes.getMaxLeadTimePlusOne());
    
    
    Cost costInState = problem.getCost();
    Demand demand=problem.getDemand();
    
    getApproximationMultiPeriodUpdate(L,T, costInState, demand, iterationNumber,leadTimes);
    
    
    //printPLValueFunction();
    
    setInitialState(costInState);
    
}

void ADPLeadTimeTransshipment_Solver::setInitialState(const Cost& cost)
{
    for (int i=0; i<IL->getLength(); i++) {
        IL->setElement(i,0,approxVF->get(0,i,0).getOptimalBreakpoint(cost.getOrderingCost()[i]));
    }
}


void ADPLeadTimeTransshipment_Solver::initializeLastPeriodApproximation(const int& time, const  Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
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


void ADPLeadTimeTransshipment_Solver::getApproximationSinglePeriodUpdate(const int& location, const int& time,const  Cost& cost,  Demand& demand, const int& iterationNumber, const LeadTimes& leadTimes)
{
    int maxLeadTime=leadTimes.getMaxLeadTimePlusOne();
    State state(location,maxLeadTime);
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,20);
    
    randomDoubleNumber randomNumberGenerator(0,1,iterationNumber);
    vector<boost::shared_ptr<randomIntNumber> > randomILGenerator(location);
    for (int j=0; j<location; j++)
    {
        boost::shared_ptr<randomIntNumber> gn(new randomIntNumber(0,IL->getElement(j,0),j));
        randomILGenerator[j]=gn;
    }
    
    
    //last period initialization
    initializeLastPeriodApproximation(time-1,cost,distribution);

    
    for (int n=0; n <iterationNumber;n++)
    {
        
        state=*IL;

        /*double random =randomNumberGenerator.sample();
        double harmonic_epsilon=(1/pow(n+1,epsilon));
        
        if (random<=harmonic_epsilon) {
            for (int i=0; i<location; i++) {
                for (int l=0; l<leadTimes.getMaxLeadTimePlusOne(); l++) {
                    state.setElement(i,l,randomILGenerator[i]->sample());
                }
            }
        }
        else
        {
            for (int i=0; i<location; i++) {
                state.setElement(i,0,approxVF->get(0,i,0).getOptimalBreakpoint(cost.getOrderingCost()[i]));
                for (int l=1; l<leadTimes.getMaxLeadTimePlusOne(); l++) {
                    state.setElement(i,l,0);
                }
            }

        }*/


        
        State post_decisionState(location,maxLeadTime);
        
        
        for (int t=0; t<time-1; t++)
        {
            post_decisionState=state;
            
            row_t profitWhatIfOneMore(location);
            row_t profitWhatIfOneLess(location);
            
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
            }
            
            getProfitOnePlus(profitWhatIfOneMore,post_decisionState,demand, cost.getHoldingCost(), cost.getSalesCost());
            getProfitOneMinus(profitWhatIfOneLess,post_decisionState,demand, cost.getHoldingCost(), cost.getSalesCost());
            
            problemMethods::getNewState(state, demand);
            
            int limit=std::min(time-t,leadTimes.getMaxLeadTimePlusOne());
            
            //std::cout<<"BREAKPOINTS:"<<'\n';
            //approxVF->printBreakpoints();
            
            //std::cout<<"SLOPES:"<<'\n';
            //approxVF->printSlopes();
            
            LP_optimizer lp_Problem;
            LP_solution decisions = lp_Problem.optimize(state, cost, approxVF, t,leadTimes,limit,true);
            
            if (lp_Problem.checkIfOptimalBasicDegenerate())
            {
                lp_Problem.getTrueShadowPrices(decisions,location,limit, leadTimes);
            }
            else
            {
                lp_Problem.getShadowPricesAsDualVariables(decisions);
                
            }
            
            
            for (int i=0; i<location; i++) {
                if (post_decisionState.getElement(i,0)<demand.getElement(i)) {
                    approxVF->updateSlopeCave(t,i, 0,profitWhatIfOneMore[i], profitWhatIfOneLess[i], post_decisionState.getElement(i,0),location,n,step);
                }
                else
                {
                    if (post_decisionState.getElement(i,0)>demand.getElement(i)) {
                        approxVF->updateSlopeCave(t,i, 0,profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i), (profitWhatIfOneLess[i]+decisions.getShadowPriceMinus(i)), post_decisionState.getElement(i,0),location,n,step);
                    }
                    else
                        approxVF->updateSlopeCave(t,i, 0, profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i), profitWhatIfOneLess[i], post_decisionState.getElement(i,0),location,n,step);
                }

                
            }
            
            /*double random =randomNumberGenerator.sample();
            double harmonic_epsilon=(1/pow(n+1,epsilon));
            
            if (random<=harmonic_epsilon) {
                
                Actions newDecision(location);
                newDecision= getNewDecision(state,n*time+t);
                problemMethods::getNewState(state);
                problemMethods::getNewState(state, newDecision,leadTimes);
                
                
                
            }
            else
            {
                problemMethods::getNewState(state);
                problemMethods::getNewState(state, decisions,leadTimes);
                
            }*/
        
    	}
    }
    
}

void ADPLeadTimeTransshipment_Solver::getApproximationMultiPeriodUpdate(const int& location, const int& time,const  Cost& cost,Demand& demand, const int& iterationNumber,const LeadTimes& leadTimes)
{
    
    int maxLeadTime=leadTimes.getMaxLeadTimePlusOne();
    State state(location,maxLeadTime);
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,20);
    
    randomDoubleNumber randomNumberGenerator(0,1,iterationNumber);
    vector<boost::shared_ptr<randomIntNumber> > randomILGenerator(location);
    for (int j=0; j<location; j++)
    {
        boost::shared_ptr<randomIntNumber> gn(new randomIntNumber(0,IL->getElement(j,0),j));
        randomILGenerator[j]=gn;
    }
    
    //last period initialization
    initializeLastPeriodApproximation(time-1,cost,distribution);
    
    
    for (int n=0; n <iterationNumber;n++)
    {
        /*std::cout<<"-----------n--------"<<n<<'\n';
        
        std::cout<<"BREAKPOINTS:"<<'\n';
        approxVF->printBreakpoints();
        
        std::cout<<"SLOPES:"<<'\n';
        approxVF->printSlopes();*/
        
        state=*IL;
        
        
        /*double random =randomNumberGenerator.sample();
        double harmonic_epsilon=(1/pow(n+1,epsilon));
        
        if (random<=harmonic_epsilon) {
            for (int i=0; i<location; i++) {
                for (int l=0; l<leadTimes.getMaxLeadTimePlusOne(); l++) {
                    state.setElement(i,l,randomILGenerator[i]->sample());
                }
            }
        }
        else
        {
            for (int i=0; i<location; i++) {
                state.setElement(i,0,approxVF->get(0,i,0).getOptimalBreakpoint(cost.getOrderingCost()[i]));
                for (int l=1; l<leadTimes.getMaxLeadTimePlusOne(); l++) {
                    state.setElement(i,l,0);
                }
            }
        
        }*/

        
        State post_decisionState(location,maxLeadTime);
        
        arrays::Array3D<float> gradientPlus(location,time,maxLeadTime+1);
        arrays::Array3D<float> gradientMinus(location,time,maxLeadTime+1);
        gradientPlus.initialize();
        gradientMinus.initialize();
        
        std::vector<State> statePath(time,State(location,maxLeadTime));
        
        for (int t=0; t<time-1; t++)
        {
            //std::cout<<"time: "<<t<<'\n';
   
            
            double random =randomNumberGenerator.sample();
            double harmonic_epsilon=(1/pow(n+1,epsilon));
             
            
            if (random<=harmonic_epsilon) {
            	 for (int i=0; i<location; i++) {
             		for (int l=0; l<leadTimes.getMaxLeadTimePlusOne(); l++) {
             			post_decisionState.setElement(i,l,randomILGenerator[i]->sample());
             		}
             		}
            }
            else
        	{
                post_decisionState=state;
             
            }
            
            
            //std::cout<<"Post decision state:"<<'\n';
            //post_decisionState.print();
            
            row_t profitWhatIfOneMore(location);
            row_t profitWhatIfOneLess(location);
            
            statePath[t]=post_decisionState;
            
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
            }
            
            //std::cout<<"Demand"<<'\n';
            //demand.print();
            
            getProfitOnePlus(profitWhatIfOneMore,post_decisionState,demand, cost.getHoldingCost(), cost.getSalesCost());
            getProfitOneMinus(profitWhatIfOneLess,post_decisionState,demand, cost.getHoldingCost(), cost.getSalesCost());
            
            problemMethods::getNewState(state, demand);
            
            //std::cout<<"New state:"<<'\n';
            //state.print();
            
            LP_optimizer lp_Problem;
            LP_solution decisions = lp_Problem.optimize(state, cost, approxVF, t,leadTimes,leadTimes.getMaxLeadTimePlusOne(),false);
            
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
                    gradientPlus(t,i,0)=profitWhatIfOneMore[i];
                    gradientMinus(t,i,0)=profitWhatIfOneLess[i];
                }
                else
                {
                    if (post_decisionState.getElement(i,0)>demand.getElement(i)) {
                        gradientPlus(t,i,0)=profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i);
                        gradientMinus(t,i,0)=profitWhatIfOneLess[i]+decisions.getShadowPriceMinus(i);
                    }
                    else
                    {
                        gradientPlus(t,i,0)=profitWhatIfOneMore[i]+decisions.getShadowPricePlus(i);
                        gradientMinus(t,i,0)=profitWhatIfOneLess[i];
                    }
                }
                    
                for (int l=1; l<maxLeadTime+1; l++) {
                    gradientPlus(t,i,l)=decisions.getShadowPricePlus(location+i*(maxLeadTime)+(l-1));
                    gradientMinus(t,i,l)=decisions.getShadowPriceMinus(location+i*(maxLeadTime)+(l-1));
                }
                    
            }
            
            /*for (int i=0; i<location; i++) {
                std::cout<<"For location: "<<i<<'\n';
                for (int l=0; l<maxLeadTime+1; l++) {
                    std::cout<<gradientPlus(t,i,l)<<" ; ";
                    //std::cout<<gradientMinus(t,i,l)<<" ; ";
                }
                std::cout<<'\n';
            }*/
            
            /*double random =randomNumberGenerator.sample();
            double harmonic_epsilon=(1/pow(n+1,epsilon));
            
            if (random<=harmonic_epsilon) {
                
                Actions newDecision(location);
                newDecision= getNewDecision(state,n*time+t);
                //std::cout<<"Decision:"<<'\n';
                //newDecision.print();
                problemMethods::getNewState(state);
                problemMethods::getNewState(state, newDecision,leadTimes);
                
            }
            else
            {*/
                problemMethods::getNewState(state);
                problemMethods::getNewState(state, decisions,leadTimes);
                //std::cout<<"Decision:"<<'\n';
                //decisions.print();
                
            //}
            
            
        }
        
        if (n>100) {
            step=15;
        }
        float updateGradientPlus=0;
        float updateGradientMinus=0;
        
        
        //std::cout<<"UPDATE:"<<'\n';
        for (int t=0; t<time-2; t++) {
            //std::cout<<"Period: "<<t<<'\n';
            for (int i=0; i<location; i++) {
                // std::cout<<"Location: "<<i<<'\n';
                
                if (t==0) {
                    updateGradientPlus=gradientPlus(t,i,0);
                    updateGradientMinus=gradientMinus(t,i,0);
                    
                    if (updateGradientMinus<updateGradientPlus) {
                        updateGradientMinus=updateGradientPlus;
                    }
                    
                    approxVF->updateSlopeCave(t,i, 0,updateGradientPlus, updateGradientMinus, statePath[t].getElement(i,0),location,n,step);
                }

                
                for (int l=1; l<maxLeadTime+1; l++) {

                    //DUALMAX
                    //updateGradientPlus=getMaxGradient(gradientPlus,t,i,l,std::min(time-1-t,l));
                    //updateGradientMinus=getMinGradient(gradientMinus,t,i,l,std::min(time-1-t,l));
                    
                    //DUALNEXT
                    updateGradientPlus=gradientPlus(t+1,i,l-1);
                    updateGradientMinus=gradientMinus(t+1,i,l-1);
                    
                    if (updateGradientMinus<updateGradientPlus) {
                        updateGradientMinus=updateGradientPlus;
                    }
                    //std::cout<<"PLUS: "<<updateGradientPlus<<'\n';
                    //std::cout<<"MINUS: "<<updateGradientMinus<<'\n';
                    
                    approxVF->updateSlopeCave(t+1,i, l-1,updateGradientPlus, updateGradientMinus, statePath[t+1].getElement(i,l-1),location,n,step);
                    //printPLValueFunction();
                }
            }
        }
        
        
        
    }
    
    
}




void ADPLeadTimeTransshipment_Solver::getProfitOnePlus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
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

void ADPLeadTimeTransshipment_Solver::getProfitOneMinus(row_t& profit,const State& state, const Demand& demand, const row_t& holdingCost, const row_t salesPrice)
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

float ADPLeadTimeTransshipment_Solver::getProfit(const int& IL, const int& d, const float& h, const float& p)
{
    return p*getMinValue(IL,d)-h*getMaxValue(IL-d,0);
}

float ADPLeadTimeTransshipment_Solver::getMaxGradient(arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit)
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

float ADPLeadTimeTransshipment_Solver::getMinGradient(arrays::Array3D<float>& gradients, const int& t, const int& i, const int& l, const int& limit)
{
    float min=std::numeric_limits<float>::max();
    for (int s=1; s<=limit; s++) {
        if (gradients(t+s,i,l-s)<min) {
            min=gradients(t+s,i,l-s);
        }
    }
    return min;
}

void ADPLeadTimeTransshipment_Solver::printPLValueFunction()
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

Actions ADPLeadTimeTransshipment_Solver::getNewDecision(const State& state, const int& rseed)
{
    srand(rseed);
    int L=state.getLength();
    Actions newDecision(L);
    int location=0;
    int amount=0;
    //min + (rand() % (int)(max - min + 1))
    for (int i=0; i<L; i++) {
        location=rand() % (int)L;
        amount=rand() % (int)(state.getElement(i,0)+1);
        if (i==location) {
            amount=0;
        }
        newDecision.setElement(i,location,amount);
    }
    return newDecision;
}
