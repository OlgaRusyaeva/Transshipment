//
//  ADPCluster_Solver.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "ADPForward_Solver.h"
#include "State.h"
#include "Actions.h"
#include "Cost.h"
#include "iDistribution.h"
#include "iProblem.h"
#include "Demand.h"
#include "ProblemMethods.h"
#include "Hashing.h"
#include "ValueFunction.h"
#include "SimulationMethods.h"
#include "array.h"
#include "ActionsGenerator.h"
#include "randomNumber.h"
#include <boost/shared_ptr.hpp>

using namespace std;

ADPForward_Solver::~ADPForward_Solver()
{
    delete valueFunction;
    delete hashFunction;
}

ValueFunction& ADPForward_Solver::getValueFunction() const
{
    return *valueFunction;
}

Hashing& ADPForward_Solver::getHashFunction() const
{
    return *hashFunction;
}


void ADPForward_Solver::initializeValueFunction()
{
    valueFunction = new ValueFunction();
}

void ADPForward_Solver::initializeHashFunction()
{
    hashFunction = new Hashing();
}

void ADPForward_Solver::solve(iProblem& problem, const int& iterationNumber,const string& f_fileName)
{
    int L = problem.getStateDimension();
    int T = problem.getTimeHorizon();
    
    State initialState = problem.getState();
    Cost costInState = problem.getCost();
    //Actions distance = problem.getDistances();
    Demand demand=problem.getDemand();
    
    getLookUpTable(L,T,initialState, costInState,demand,iterationNumber);
    
    outputLookUpTable(f_fileName);
    
}



void ADPForward_Solver::getLookUpTable(const int& location, const int& time, const State& initialState,const Cost& cost,  Demand& demand, const int& iterationNumber)
{
    vector<boost::shared_ptr<iDistribution> > distribution;
    distribution=problemMethods::initializeDemandDistribution(demand.getDistribution(), demand.getDemandParameters(),location,20);
    
    /*cout<<"mean: "<<'\n';
    for (int i=0; i<location; i++) {
        cout<<distribution[i]->mean()<<'\n';
    }*/
    
    State state(location);
    
    size_t initialStateHash=0;
    
    float alpha_n=0;
    
    randomDoubleNumber randomNumberGenerator(0,1,iterationNumber);
    vector<boost::shared_ptr<randomIntNumber> > randomILGenerator(location);
    
    
    for (int i=0; i<location; i++) {
        boost::shared_ptr<randomIntNumber> gn(new randomIntNumber(0,initialState.getElement(i,0),i));
        randomILGenerator.push_back(gn);
    }
    
    for (int n=0; n <iterationNumber;n++)
    {
        //std::cout<<"Iteration: "<<n<<'\n';
        alpha_n=alpha/(alpha+n-1);
        
        /*cout<<"Demamnd sample for solver:"<<'\n';
        for (int i=0; i<possibleDemands.sizeH(); i++) {
            for (int j=0; j<possibleDemands.sizeW(); j++) {
                cout<<possibleDemands(i,j)<<" ";
            }
            cout<<'\n';
        }*/
        
        for (int j=0; j<=location-1; j++) 
        {
            //state.setElement(j,randomILGenerator[j]->sample());
            state.setElement(j,0,initialState.getElement(j,0));
            
        }
        
        for (int t=0; t<time; t++) 
        {
            //std::cout<<"Time: "<<t<<'\n';
                        
            /*cout<<"State: "<<'\n';
            for (int i=0; i<location; i++) {
                cout<<state.getElement(i)<<'\n';
            }*/
            
            arrays::Array2D<float> possibleDemands(location,demandNumber);
            possibleDemands.initialize();
            getDemandSample(possibleDemands,distribution);
            
        
            initialStateHash = hashFunction->hash_value(state);
            //cout<<"Hash of pre-decision state: "<<hashFunction->hash_value(state)<<'\n';
            
            float transshipmentCost=0;

            transshipmentCost=getCostForActions(state,t,cost,time,possibleDemands);
                //updateValueFunction(reward-holdingCost+transshipmentCost,initialStateHash,t,alpha_n);
            
            /*std::cout<<"State:"<<'\n';
            for (int i=0; i<location; i++) {
                std::cout<<state.getElement(i)<<'\n';
            }*/
                
            //float probability=1;

            //std::cout<<"Demand:"<<'\n';
            for (int l=0; l<location; l++) {
                demand.setElement(l, (int)distribution[l]->sample());
                //probability*=distribution[l]->pdf(demand.getElement(l));
                //std::cout<<demand.getElement(l)<<'\n';
            }
            
            updateValueFunction(transshipmentCost,initialStateHash,t,alpha_n);
            initialStateHash=0;
            
            double random =randomNumberGenerator.sample();
            
            //with small probability epsilon pick up the next location randomly
            if (random<=epsilon) {
             	//cout<<"random"<<'\n';
             	for (int j=0; j<=location-1; j++) 
             	{
             		state.setElement(j,0,randomILGenerator[j]->sample());
             	
             	}
             }
            else
                problemMethods::getNewState(state, demand);

            
        }
        
    }
    
}


void ADPForward_Solver::outputLookUpTable(const string& f_fileName)
{
    ofstream file_output(f_fileName.c_str());
    //ofstream file_output ("/Users/orusyaeva/Documents/ADP_Project/ADP/TransshipmentProject/TransshipmentProject/output");    
    valueFunction->printVFunction(file_output);
    file_output.close();
}


void ADPForward_Solver::updateValueFunction(const float& cost, const size_t& initialStateHash, const int& t, const float& alpha_n)
{
    float updateCost=0;
    updateCost=(1-alpha_n)*valueFunction->getValueFunction(t, initialStateHash)+alpha_n*(cost);
    valueFunction->setValueFunction(t,initialStateHash,updateCost); 
}

void ADPForward_Solver::getDemandSample(arrays::Array2D<float>& possibleDemands, const std::vector<boost::shared_ptr<iDistribution> >& distribution)
{
    for (int i=0; i<possibleDemands.sizeH(); i++) {
        for (int j=0; j<possibleDemands.sizeW(); j++) {
            possibleDemands(i,j)=distribution[j]->sample();
        }
    }
    
    
}
