//
//  SimulatorOneTransshipment.cpp
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#include "SimulatorOneTransshipment.h"
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
#include "LP_optimizer.h"
#include <math.h>


using namespace myopicMethods;

SimulatorOneTransshipment::~SimulatorOneTransshipment()
{
    //delete F;
}


void SimulatorOneTransshipment::initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction)
{
    F=NoTransshipmentFunction;
}

void SimulatorOneTransshipment::simulate(iProblem& problem,Profit& profit)
{
    int location = problem.getStateDimension();
    int time = problem.getTimeHorizon();

    
    State initialState = problem.getState();
    Cost cost = problem.getCost();
    Demand demand=problem.getDemand();
    
    //int transshipmentSizeTotal=0;
    matrixInt_t totalTransshipmentMatrix = initializeTransshipmentMatrix(location,location);
    
    std::vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,seed);
    
    State state(location);
    
    //matrixInt_t demandMatrix=initializeTransshipmentMatrix(time,location);
    
    //std::cout<<"---------------"<<'\n';
    
    for (int n=0; n <iterationNumber;n++)
    {

        //generateDemand(location,time,demandMatrix,n,replicationNumber,distribution);
        int transshipmentSizePerIteration=0;
        bool transshipmentDone=false;
        
        //std::cout<<"n: "<<n<<'\n';
        float totalProfit = 0;
        
        state=initialState;
        
        for (int t=0; t<time; t++)
        {
            int transshipmentSizePerPeriod=0;
            int transshipmentNumberPerPeriod=0;
            
            //std::cout<<t<<'\n';
            
            //transship at certain period defined in input
            //OR
            //find time to transship
            if ((!transshipmentDone) && timeToTransship(problem,t,state,n)) {
                

                
                /*std::cout<<"State:"<<'\n';
                 for (int i=0; i<location; i++) {
               	 std::cout<<state.getElement(i)<<'\n';
                 }*/
                
                float transshipmentCost=0;
                
                Actions maxTransshipmentMatrix(location);
                
                transshipmentCost=getTransshipmentCost(cost, t, state,maxTransshipmentMatrix);
                
                // std::cout<<"Transshipments:"<<'\n';
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
                
                totalProfit-=transshipmentCost;
                
                /*std::cout<<"State after:"<<'\n';
                 for (int i=0; i<location; i++) {
                 std::cout<<state.getElement(i)<<'\n';
                 }*/
                
                transshipmentDone=true;
            }
            
            //std::cout<<"Demand:"<<'\n';
            for (int i=0; i<location; i++) {
                demand.setElement(i, (int)distribution[i]->sample());
                //demand.setElement(i,demandMatrix[t][i]);
                //std::cout<<demand.getElement(i)<<'\n';
            }
            
            float reward = problemMethods::getReward(cost.getSalesCost(),state,demand);
            
            problemMethods::getNewState(state, demand);
            
            float holdingCost = problemMethods::getCost(cost.getHoldingCost(),state);
            
            totalProfit+=reward-holdingCost;
            
            //std::cout<<"Profit: "<<reward-holdingCost-transshipmentCost<<'\n';
            
            //no transshipments first time period
            profit.setTransshipmentSizePerPeriod(t,transshipmentSizePerPeriod+profit.getTransshipmentSizePerPeriod(t));
            profit.setFrequency(t,transshipmentNumberPerPeriod+profit.getFrequency(t));
            
            transshipmentSizePerIteration+=transshipmentSizePerPeriod;
            
            if (t==time-1) {
                for (int i=0; i<location; i++) {
                    profit.setLeftInventory(profit.getLeftInvetory(i,0)+state.getElement(i,0),i,0);
                }
            }
            
            
        }
        
        profit.setProfit(n,totalProfit);
        profit.setTransshipmentSizePerIteration(n,transshipmentSizePerIteration);
        //transshipmentSizeTotal+=transshipmentSizePerIteration;
        //to change seed for each replication
        //problemMethods::resetSimulationParameters(distribution,seed,location,n);

    }
    profit.setTransshipmentMatrix(totalTransshipmentMatrix);
    
    
}


float SimulatorOneTransshipment::getTransshipmentCost(const Cost& cost,const int& currentPeriod, State& currentState,Actions& maxTransshipmentMatrix)
{
    //int time=problem.getTimeHorizon();
    
    LP_optimizer lp_Problem;
    //maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, PVF->getIndexVector(0), PVF->getBreakpoints(0), PVF->getSlope(0));
    maxTransshipmentMatrix = lp_Problem.optimize(currentState, 0,cost, F,currentPeriod);
    
    problemMethods::getNewState(currentState, maxTransshipmentMatrix);
    
    float transshipmentCost = problemMethods::getTransshipmentCost(cost,maxTransshipmentMatrix);
    
    return transshipmentCost;
    
}

//change to get an optimal value then we don't need getExpectedProfitNextPeriodsWithoutTransshipments
float SimulatorOneTransshipment::getExpectedProfitWithCurrentTransshipment(const Cost& cost,const int& currentPeriod, const State& currentState, bool& isZero)
{
    //int time=problem.getTimeHorizon();
    Actions maxTransshipmentMatrix(currentState.getLength());

    LP_optimizer lp_Problem;
    //maxTransshipmentMatrix = lp_Problem.optimize(currentState, cost, PVF->getIndexVector(0), PVF->getBreakpoints(0), PVF->getSlope(0));
    maxTransshipmentMatrix = lp_Problem.optimize(currentState, 0,cost, F,currentPeriod);

    
    if (maxTransshipmentMatrix.isZero())
    {
        isZero=true;
    }
    
    //problemMethods::getNewState(currentState, maxTransshipmentMatrix);
    
    //float transshipmentCost = problemMethods::getTransshipmentCost(cost,maxTransshipmentMatrix);
    
    return lp_Problem.optimalObjectiveFunction();
    
}


float SimulatorOneTransshipment::getExpectedProfitNextPeriodsWithoutTransshipments(const int& location,const int& currentPeriod, const State& currentState)
{
    
    float expectedProfitNextPeriodsWithoutTransshipments=0;
    for (int i=0; i<location; i++) {
        for (int j=0; j<F->get(currentPeriod,i).getBreakpointClosestTo(currentState.getElement(i,0)); j++) {
            //check
            expectedProfitNextPeriodsWithoutTransshipments+=F->get(currentPeriod,i).getSlope(F->get(currentPeriod,i).getBreakpoint(j));
        }
    }
    
    return expectedProfitNextPeriodsWithoutTransshipments;
    
    
    /*int location = problem.getStateDimension();
    Cost cost = problem.getCost();
    int time = problem.getTimeHorizon();
    
    float expectedProfitNextPeriodsWithoutTransshipments=0;
    for (int i=0; i<location; i++) {
        float expectedShortage=0;
        for (int t=0; t<time-currentPeriod; t++) {
            expectedShortage+=expectedValuePositivePart(t+1,distribution[i],currentState.getElement(i));
        }
        expectedProfitNextPeriodsWithoutTransshipments+=(time-currentPeriod)*(cost.getSalesCost()[i]*distribution[i]->mean()+(float)(time-currentPeriod+1)/2*cost.getHoldingCost()[i]*distribution[i]->mean()-cost.getHoldingCost()[i]*currentState.getElement(i))-cost.getHoldingCost()[i]*expectedShortage-cost.getSalesCost()[i]*expectedValuePositivePart(time-currentPeriod,distribution[i],currentState.getElement(i));
    }
    return expectedProfitNextPeriodsWithoutTransshipments;*/
}

/*void SimulatorOneTransshipment::generateDemand(const int& location, const int& time, matrixInt_t& demandMatrix, const int& currentIteration, const int& replicationNumber, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    for (int t=0; t<time; t++) {
        if ((fmod(currentIteration,(int)pow(replicationNumber,time-t-1)))==0) {
            for (int i=0; i<location; i++) {
                demandMatrix[t][i]=(int)distribution[i]->sample();
            }
        }
    }
}*/
/*void Simulator::setResults(row_t& solution, Profit& profit)
 {
	profit.setData(solution);
 }*/


//get all Locations that out of stock after demand satisfaction
vector_t SimulatorOneTransshipment::getStockOutLocations(const State& initialState)
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
matrixInt_t SimulatorOneTransshipment::initializeTransshipmentMatrix(const int& size_row, const int& size_col)
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
void SimulatorOneTransshipment::printMatrix(const int& limit1, const int& limit2,const matrixInt_t& M)
{
    for (int i=0; i<= limit1-1; i++) {
        for (int j=0; j<=limit2-1; j++) {
            std::cout << std::setw(10)<< M[i][j];
        }
        std::cout << '\n';
    }
    
}
