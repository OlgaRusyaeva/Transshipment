//
//  TransshipmentProblem.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 11/5/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "TransshipmentProblem.h"
//#include "Actions.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "Output.h"
#include "Input.h"
#include "LeadTimes.h"
#include <fstream>
#include "randomNumber.h"
#include <boost/shared_ptr.hpp>

//#include <math.h>
//#include <ctime>
//#include <iostream>
//#include <string>

using namespace std;

TransshipmentProblem::TransshipmentProblem(const Input& input, const int& scenarioNumber)
{
    initialize(input,scenarioNumber);
}
    
TransshipmentProblem::~TransshipmentProblem()
{
    delete state;
    delete demand;
    delete cost;
    delete leadTimes;
    //delete actions;
}
const State& TransshipmentProblem::getState() const
{
    return *state;
}
    
const Demand& TransshipmentProblem::getDemand() const
{
    return *demand;
}

    
const int TransshipmentProblem::getStateDimension() const
{
    return Location;
}

const int TransshipmentProblem::getLifeTime() const
{
    return LifeTime;
}
    
const int TransshipmentProblem::getTimeHorizon()const
{
    return Time;
}

const LeadTimes& TransshipmentProblem::getLeadTimes() const
{
    return *leadTimes;
}

const matrixInt_t TransshipmentProblem::getIndenticalLocations() const
{
    return identical;
}
    
/*const Actions& TransshipmentProblem::getDistances() const
{
    return *actions;
}*/

const Cost& TransshipmentProblem::getCost() const
{
    return *cost;
}
    
void TransshipmentProblem::outputInput(const string& fileName)const
{
 	outputInitialData(fileName);   
    output::outputMap(*state,*cost);
}
    
/*    
void TransshipmentProblem::readInput(const string& f_fileName)
{
        ifstream inputFile(f_fileName.c_str());
        if (inputFile == NULL) perror ("Error opening file.");
        else
        {
            inputFile >> Location; //number of locations
            inputFile >> Time; //number of periods
        }
        inputFile.close();
        
}
    
void TransshipmentProblem::readInputCost(const string& f_fileName)
{
        std::string line;
        initializeCost();
        ifstream inputFile(f_fileName.c_str());
        if (inputFile == NULL) perror ("Error opening file.");
        else
        {
            int costCounter=0;
            while(std::getline(inputFile, line))
            {
                std::stringstream ss(line);
                int i;
                int locationCounter=0;
                while( ss >> i ) 
                {
                    //cout<<costCounter<<'\n';
                    cost->setCost(costCounter, locationCounter, i);
                    locationCounter++;
                }
                if ((locationCounter>1)&&(locationCounter<Location))
                    perror ("Error in the input. Check inputCost file");
                if ((locationCounter==1)&&(Location>1)) {
                    for (int j=1; j<Location; j++) {
                        cost->setCost(costCounter, j, i);
                    }
                }
                costCounter++;
                
            }
            
        }
        inputFile.close();
        
}
    
void TransshipmentProblem::readInputDemandParameters(const string& f_fileName)
{
        string line;
        initializeDemand();
        ifstream inputFile(f_fileName.c_str());
        if (inputFile == NULL) perror ("Error opening file.");
        else
        {
            int locationCounter=0;
            while(std::getline(inputFile, line))
            {
                vector<float> inputVector;
                istringstream iss(line);
                float value;
                while (iss >> value)
                {
                    inputVector.push_back(value);
                }
                if ((int)inputVector.size()==1) {
                    inputVector.push_back(value);
                }
                demand->setDemandParameters(locationCounter,inputVector[0],inputVector[1]);
                locationCounter++;
                //demandParameters[i]=make_pair(inputVector[0], inputVector[1]);
            }
            if (locationCounter!=Location)
                perror ("Error in the input. Check inputDemandParameter file");
            
        }
        inputFile.close();
        
}
    
void TransshipmentProblem::readInputIL(const string& f_fileName)
{
        int inventoryLimit=0;
        initializeState();
        string line;
        ifstream inputFile(f_fileName.c_str());
        if (inputFile == NULL) perror ("Error opening file.");
        else
        {
            int locationCounter=0;
            while(std::getline(inputFile, line))
            {
                istringstream iss(line);
                iss >> inventoryLimit;
                state->setInitialState(locationCounter,inventoryLimit); // constant inventory level
                locationCounter++;
            }
            if (locationCounter!=Location)
                perror ("Error in the input. Check inputInitialState file");
        }
        inputFile.close();
        
}
*/

void TransshipmentProblem::initialize(const Input& input, const int& scenarioNumber)
{
    initializeLocationNumber(input.getNumberLocations(scenarioNumber));
    initializeTimePeriodsNumber(input.getNumberTimePeriods(scenarioNumber));
    initializeLifeTimeNumber(input.getMaxLifeTime(scenarioNumber));
    initializeLeadTimes(input.getLeadTimes(scenarioNumber));
	initializeState(input.getInitialIL(scenarioNumber),input.getMaxLifeTime(scenarioNumber));
    initializeDemand(input.getDemandParameter1(scenarioNumber),input.getDemandParameter2(scenarioNumber),input.getDistribution(scenarioNumber));
    initializeCost(input.getHoldingCost(scenarioNumber),input.getPrice(scenarioNumber),input.getTransshipmentCost(scenarioNumber),input.getOrderingCost(scenarioNumber));
    initializeCoordinates(input.getCoordinateX(scenarioNumber),input.getCoordinateY(scenarioNumber));
    initializeDistances(input.getDistance(scenarioNumber));
    findIdenticalLocations();
    
}

void TransshipmentProblem::findIdenticalLocations()
{
    identical=matrixInt_t(Location);
    
    if (Location>0) {
        identical[0].push_back(0);
    }
    
    bool flag;
    for (int i=1; i<Location; i++) {
        flag=false;
        for (int j=0; j<=i-1; j++) {
            if ((cost->getHoldingCost()[j]==cost->getHoldingCost()[i])&&(cost->getSalesCost()[j]==cost->getSalesCost()[i])&&(demand->getDistribution()[j]==demand->getDistribution()[i])&&(demand->getDemandParametersForLocation(j).first==demand->getDemandParametersForLocation(i).first)&&(demand->getDemandParametersForLocation(j).second==demand->getDemandParametersForLocation(i).second))
            {
                flag=true;
                identical[j].push_back(i);
                break;
            }
                
        }
        if (flag==false) {
            identical[i].push_back(i);
        }
    }
}

void TransshipmentProblem::initializeLocationNumber(const int& numberLocations)
{
	Location=numberLocations;
}

void TransshipmentProblem::initializeTimePeriodsNumber(const int& numberPeriods)
{
    Time=numberPeriods;
}

void TransshipmentProblem::initializeLifeTimeNumber(const int& maxLifeTime)
{
    LifeTime=maxLifeTime+1;
}

void TransshipmentProblem::initializeLeadTimes(const matrixInt_t& leadTimeMatrix)
{
    leadTimes=new LeadTimes(leadTimeMatrix);

}


void TransshipmentProblem::initializeState(const vector_t& IL,const int& maxLifeTime)
{
    /*vector_t maxLeadTimes(Location);
    for (int i=0; i<Location; i++) {
        maxLeadTimes[i]=(std::max_element(leadTimes[i].begin(),leadTimes[i].end())-leadTimes[i].begin()-1);
    }*/
    
    //TODO: solve the perishable problem with non-zero lead times
    //NOW: either we take into account the life time or the transshipment lead time
    if (maxLifeTime==0) {
        state = new State(Location,leadTimes->getMaxLeadTimePlusOne());
        
        for (int i=0; i<Location; i++) {
            state->setElement(i,0,IL[i]);
        }
    }
    else
    {
        state = new State(Location,1,maxLifeTime+1);
        
        vector<boost::shared_ptr<randomIntNumber> > randomLifeTimeGenerator(Location);
        for (int j=0; j<Location; j++)
        {
            boost::shared_ptr<randomIntNumber> gn(new randomIntNumber(0,maxLifeTime,j));
            randomLifeTimeGenerator[j]=gn;
        }
        
        for (int i=0; i<Location; i++) {
            for (int inv=0; inv<IL[i]; inv++) {
                int lifeTime=randomLifeTimeGenerator[i]->sample();
                state->setElement(i,lifeTime,state->getElement(i,lifeTime)+1);
            }
        }
    }

}
    
void TransshipmentProblem::initializeDemand(const row_t& param1, const row_t& param2, const vector_t& distribution)
{
    demand = new Demand(Location);
    for (int i=0; i<Location; i++) {
    	demand->setDemandParameters(i,param1[i],param2[i]);
        demand->setDistribution(i,distribution[i]);
    }


}
void TransshipmentProblem::initializeCost(const row_t& h, const row_t& p, const row_t& c, const row_t& od)
{
    cost = new Cost(Location);
    for (int i=0; i<Location; i++) {
		cost->setSalesCost(i,p[i]);
        cost->setHoldingCost(i,h[i]);
        cost->setTransshipmentCost(i,c[i]);
        cost->setOrderingCost(i,od[i]);
    }
}

    
void TransshipmentProblem::initializeCoordinates(const row_t& x, const row_t& y)
{
        //srand(time(NULL));
        //base_gen_type generator(78);//fix the number to reapeat the same random numb every run
        //gen_uniform_int_AB unif_gen_0100(generator, uniform_int_AB(0,100));
        //generateCoordinates(unif_gen_0100);
    if (cost!=NULL) {
        for (int i=0; i<=Location-1; i++) {
            cost->setXCoordinates(i, x[i]);
            cost->setYCoordinates(i, y[i]);
        }
    }
        
}
void TransshipmentProblem::initializeDistances(const matrix_t& d)
{
    if (cost!=NULL) {
        cost->setDistances(d);
    }
    
}



/*void TransshipmentProblem::generateCoordinates(gen_uniform_int_AB& gen)
{
    if (Location==2) {
        //F x 80 20
        //F y 45 34
        //C x 80 59
        //C y 60 40
        x.push_back(80);
        x.push_back(20);
        y.push_back(45);
        y.push_back(34);
    }
    else
    {
        // separate or not? 
        for (int i=0; i<=Location-1; i++) {
            x.push_back(gen());
            y.push_back(gen());
        }
        for (int i=0; i<=Location-1; i++) {
            if ((x[i]==0)&&(y[i]==0)){
                generateCoordinates(gen);
            }
            for (int j=0; j<=Location-1; j++) {
                if ((i!=j)&&(x[i]==x[j])&&(y[i]==y[j])) {
                    generateCoordinates(gen);
                }
            }
        }
    }
}*/
    
/*float TransshipmentProblem::distance(const int& stateA, const int& stateB)const
{
    return sqrtf(pow(((float)(x[stateA]-x[stateB])), 2)+pow(((float)(y[stateA]-y[stateB])), 2)); 
}*/
    
void TransshipmentProblem::outputInitialData(const string& fileName)const
{
    	output::cleanFile(fileName);
    
        ofstream outputFile;
        outputFile.open(fileName.c_str(),ios::out);
        outputFile<<"Number of locations: "<<Location<<'\n';
        outputFile<<"Time Horizon: "<<Time<<'\n';
        for (int i=0; i<Location; i++) {
            outputFile<<"Location: "<<i<<'\n';
            outputFile<<"Initial IL: "<<state->getElement(i,0)<<'\n';
            outputFile<<"Ordering cost: "<<cost->getOrderingCost()[i]<<'\n';
            outputFile<<"Holding cost: "<<cost->getHoldingCost()[i]<<'\n';
            outputFile<<"Transshipment cost: "<<cost->getTransshipmentCost()[i]<<'\n';
            outputFile<<"Sales cost: "<<cost->getSalesCost()[i]<<'\n';
            outputFile<<"Demand parameter 1: "<<demand->getDemandParametersForLocation(i).first<<'\n';
            outputFile<<"Demand parameter 2: "<<demand->getDemandParametersForLocation(i).second<<'\n';
            for (int j=0; j<Location; j++) {
                outputFile<<"Distance from location "<<i<<" to location "<<j<<" : "<<cost->getDistance(i,j)<<'\n';
            }
        } 
        outputFile.close();
}
    
    
