//
//  DPLeadTimeTransshipment_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga on 16/09/15.
//
//

#include "DPLeadTimeTransshipment_Solver.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "ProblemMethods.h"
#include "Output.h"
#include "LeadTimes.h"
#include "MyopicMethods.h"
#include "SimulationMethods.h"
#include "array.h"
#include "NumbersOperations.h"
#include "Actions.h"
#include "ActionsGenerator.h"

using namespace std;
using namespace arrays;
using namespace actions;

DPLeadTimeTransshipment_Solver::~DPLeadTimeTransshipment_Solver()
{
    delete valueFunction;
    delete hashFunction;
}

ValueFunction& DPLeadTimeTransshipment_Solver::getValueFunction() const
{
    return *valueFunction;
}

Hashing& DPLeadTimeTransshipment_Solver::getHashFunction() const
{
    return *hashFunction;
}

boost::shared_ptr<State> DPLeadTimeTransshipment_Solver::getInitialState() const
{
    return initialState;
}

void DPLeadTimeTransshipment_Solver::initializeValueFunction()
{
    valueFunction = new ValueFunction();
}

void DPLeadTimeTransshipment_Solver::initializeHashFunction()
{
    hashFunction = new Hashing();
}

void DPLeadTimeTransshipment_Solver::initializeState(const boost::shared_ptr<State>& upperBound)
{
    initialState.reset(new State(*upperBound));
}

void DPLeadTimeTransshipment_Solver::solve(const string& f_fileNameVF, const string& f_fileNameIL)
{
    ifstream file_inputVF(f_fileNameVF.c_str());
    valueFunction->inputVFunction(file_inputVF);
    file_inputVF.close();
    
    ifstream file_inputIL(f_fileNameIL.c_str());
    initialState->inputState(file_inputIL);
    file_inputIL.close();
}

void DPLeadTimeTransshipment_Solver::solve(iProblem& problem,const string& f_fileNameVF,const string& f_fileNameIL)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    
    Cost costInState = problem.getCost();
    Demand demand=problem.getDemand();
    LeadTimes leadtimes=problem.getLeadTimes();
    
    setLookUpTable(L,T, costInState, demand, leadtimes);
    
    outputLookUpTable(f_fileNameVF);
    
    outputInitialState(f_fileNameIL);
    
}

void DPLeadTimeTransshipment_Solver::outputInitialState(const string& f_fileNameIL)
{
    ofstream file_output(f_fileNameIL.c_str(), ios::out | ios::app);
    initialState->outputState(file_output);
    file_output.close();
}

void DPLeadTimeTransshipment_Solver::outputLookUpTable(const string& f_fileNameVF)
{
    ofstream file_output(f_fileNameVF.c_str(), ios::out | ios::app);
    valueFunction->printVFunction(file_output);
    file_output.close();
}


void DPLeadTimeTransshipment_Solver::setLookUpTable(const int& location, const int& time, const Cost& cost, Demand& demand, const LeadTimes& leadtimes)
{    
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,0);
    
    State state(location,leadtimes.getMaxLeadTimePlusOne());
    State maxState(location,1);
    float maxValue=0;
    
    int maxIL=getUpperBoundForIL();
    int size_inventoryLevels=1;
    for (int i=0; i<location; i++) {
        for (int t=0; t<leadtimes.getMaxLeadTimePlusOne(); t++) {
            size_inventoryLevels*=maxIL;
        }
    }
    
    size_t initialStateHash=0;
    
    //for each time period
    for (int t=time-1; t>=0; t--)
    {
        
        cout<<"time: "<<t<<'\n';
        
        int inv=0;
        
        //for each state in the state space
        while (inv<size_inventoryLevels) {
            
            state=simulationMethods::generateInventoryLevel(maxIL,inv,location,leadtimes.getMaxLeadTimePlusOne());
            
            //find better solution
            //the sum inventories of possible state can't be more than the sum inventories of initial state
            //if the initial state is given
            if (proceedState(state,leadtimes,maxIL)) {
                
                initialStateHash = hashFunction->hash_value(state);
                
                
                float value=0;
                if (t==time-1) {
                    value=getLastPeriodValueFunction(state,cost,distribution);
                }
                else
                    value=getExpectedProfit(t,time,state,demand,cost,distribution,leadtimes);
                
                int aggregateInvPipeline=0;
                for (int j=0; j<location; j++) {
                    for (int l=1; l<leadtimes.getMaxLeadTimePlusOne(); l++) {
                        aggregateInvPipeline+=state.getElement(j,l);
                    }
                }

                
                if ((t==0)&&(aggregateInvPipeline==0)) {
                    float orderingCost=problemMethods::getCost(cost.getOrderingCost(),state);
                    if (value-orderingCost>maxValue) {
                        maxValue=value-orderingCost;
                        maxState=state;
                    }

                }
                
                valueFunction->setValueFunction(t,initialStateHash,value);
                initialStateHash = 0;
            }
            
            
        }
        
        
    }
    
    for (int i=0; i<location; i++) {
        initialState->setElement(i,0,maxState.getElement(i,0));
    }
    
}

bool DPLeadTimeTransshipment_Solver::proceedState(const State& state, const LeadTimes& leadTimes, const int& maxIL)
{
    bool proceed=true;
    int location=state.getLength();
    
    int aggregateInvCurrent=0;
    int aggregateInvPipeline=0;
    int aggregateInvOutsideBorders=0;
    
    //initialize state
    for (int j=0; j<=location-1; j++)
    {
        aggregateInvCurrent+=state.getElement(j,0);
        for (int l=1; l<leadTimes.getMaxLeadTimePlusOne(); l++) {
            if (l>leadTimes.getMaxLeadTimeForLocation(j)) {
                aggregateInvOutsideBorders+=state.getElement(j,l);
            }
            else
                aggregateInvPipeline+=state.getElement(j,l);
        }
        
    }
    
    if ((aggregateInvOutsideBorders>0)||(aggregateInvCurrent+aggregateInvPipeline)>maxIL) {
        proceed=false;
    }
    
    return proceed;
}

//TODO
int DPLeadTimeTransshipment_Solver::getUpperBoundForIL()
{
    int ub=0;
    for (int i=0; i<initialState->getLength(); i++) {
        ub+=initialState->getElement(i,0);
    }
    return ub;
}

float DPLeadTimeTransshipment_Solver::getLastPeriodValueFunction(const State& state, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    float lastPeriodProfit=0;
    for (int i=0; i<state.getLength(); i++) {
        lastPeriodProfit+=(cost.getSalesCost()[i]+cost.getHoldingCost()[i])*(distribution[i]->mean()-myopicMethods::expectedValuePositivePart(1,distribution[i],state.getElement(i,0)))-cost.getHoldingCost()[i]*state.getElement(i,0);
    }
    return lastPeriodProfit;
    
}

float DPLeadTimeTransshipment_Solver::getExpectedProfit(const int& t, const int& time, const State& state, Demand& demand, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const LeadTimes& leadTimes) const
{
    const int location=state.getLength();
    float expectedProfit=0;
    //for each demand realization
    State minBorder(location);
    int size_demands=1;
    for (int i=0; i<location; i++) {
        minBorder.setElement(i,0,state.getElement(i,0));
        size_demands*=state.getElement(i,0)+1;
    }
    Array2D<float> possibleDemands(location,size_demands);
    possibleDemands.initialize();
    simulationMethods::generatePossibleDemands(minBorder,possibleDemands);
    
    for (int i=0; i<possibleDemands.sizeH(); i++)
    {
        //calculate probability
        float prob=1;
        bool flag;
        for (int j=0; j<location; j++) {
            flag=true;
            demand.setElement(j, (int)possibleDemands(i,j));
            if ((state.getElement(j,0)-demand.getElement(j))==0) {
                flag=false;
            }
            prob*=problemMethods::probabilityOfDemand(demand.getElement(j),distribution[j],flag);
        }
        
        //get reward
        float reward = problemMethods::getReward(cost.getSalesCost(),state,demand);
        //cout<<"reward: "<<reward<<'\n';
        
        State stateAfterDemandSatisfaction(location,leadTimes.getMaxLeadTimePlusOne());
        stateAfterDemandSatisfaction=state;
        
        problemMethods::getNewState(stateAfterDemandSatisfaction, demand);//!!!
        
        //get holding cost
        float holdingCost=problemMethods::getCost(cost.getHoldingCost(), stateAfterDemandSatisfaction);
        //cout<<"Holding cost:"<<holdingCost<<'\n';
        
        float maxProfit=-numeric_limits<float>::max();
        
        //enumerate all possible actions
        vecMatrixInt actionsForState=transhipmentFromState(stateAfterDemandSatisfaction,0,false);
        
        State indexVector(location);
        int size_actions=1;
        
        simulationMethods::getIndexVector(indexVector, actionsForState,size_actions);
        
        
        Array2D<float> combinationsOfRowsActionMatrix(location, size_actions);
        combinationsOfRowsActionMatrix.initialize();
        simulationMethods::generatePossibleDemands(indexVector,combinationsOfRowsActionMatrix);
        
        //State maxState(location);
        
        //row_t costInPeriods(combinationsOfRowsActionMatrix.size());
        //#pragma omp parallel for shared(costInPeriods,actionsForState,combinationsOfRowsActionMatrix)
        for (int p=0; p<combinationsOfRowsActionMatrix.sizeH(); p++) {
            float profitInPeriod =0;
            State newState(location,leadTimes.getMaxLeadTimePlusOne());
            Actions transshipmentMatrix(location);
            newState=stateAfterDemandSatisfaction;
            problemMethods::getNewState(newState);
            for (int j=0; j<=location-1; j++) {
                for (int k=0; k<=location-1; k++) {
                    int amountToTransship=0;
                    int index=(int)combinationsOfRowsActionMatrix(p,j);
                    if(k!=j) amountToTransship=actionsForState[j][index][k];
                    newState.setElement(k,leadTimes.getLeadTimeBetweenLocations(j,k),newState.getElement(k,leadTimes.getLeadTimeBetweenLocations(j,k))+amountToTransship);
                    newState.setElement(j,0,newState.getElement(j,0)-amountToTransship);
                    transshipmentMatrix.setElement(j,k,amountToTransship);
                    
                }
            }
            profitInPeriod-=problemMethods::getTransshipmentCost(cost, transshipmentMatrix);
    		//find an action that has the highest profit
        
        	//std::cout<<"hash: "<<hashFunction->hash_value(stateAfterDemandSatisfaction)<<'\n';
        	profitInPeriod+=valueFunction->getValueFunction(t+1, hashFunction->hash_value(newState));
            
            if (profitInPeriod>=maxProfit) {
                maxProfit=profitInPeriod;
                //maxState=stateAfterDemandSatisfaction;
            }
        }
        
        //cout<<"value next period: "<<valueNextPeriod<<'\n';
        //get value function
        
        expectedProfit+=prob*(reward-holdingCost+maxProfit);
        
    }
    return expectedProfit;

}
