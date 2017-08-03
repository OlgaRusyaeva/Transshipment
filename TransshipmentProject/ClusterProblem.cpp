//
//  ClusterProblem.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "ClusterProblem.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "Output.h"
#include "Cluster.h"

using namespace std;

ClusterProblem::ClusterProblem(const iProblem& mainProblem,const vector_t& clusterVector)
{
    cluster=vector_t((int)clusterVector.size());
    cluster=clusterVector;
    
    initialize(mainProblem);
    initializeState(mainProblem);
    initializeDemand(mainProblem);
    initializeCost(mainProblem);
    initializeCoordinates(mainProblem);
}

ClusterProblem::~ClusterProblem()
{
    delete state;
    delete demand;
    delete cost;
    //delete actions;
}
const State& ClusterProblem::getState() const
{
    return *state;
}

const Demand& ClusterProblem::getDemand() const
{
    return *demand;
}
const int ClusterProblem::getStateDimension() const
{
    return Location;
}

const int ClusterProblem::getTimeHorizon()const
{
    return Time;
}

/*const Actions& ClusterProblem::getDistances() const
{
    return *actions;
}*/
const Cost& ClusterProblem::getCost() const
{
    return *cost;
}

void ClusterProblem::outputInput(const string& fileName)const
{
	outputInitialData(fileName);   
}


void ClusterProblem::initialize(const iProblem& mainProblem)
{
    Time=mainProblem.getTimeHorizon();
    Location=(int)cluster.size();;
}

void ClusterProblem::initializeState(const iProblem& mainProblem)
{
    state = new State(Location);
    for (int i=0; i<Location; i++) {
        state->setElement(i,mainProblem.getState().getElement(cluster[i]));
    }
}

void ClusterProblem::initializeDemand(const iProblem& mainProblem)
{
    demand = new Demand(Location);
    for (int i=0; i<Location; i++) {
        demand->setDemandParameters(i,mainProblem.getDemand().getDemandParametersForLocation(cluster[i]).first,mainProblem.getDemand().getDemandParametersForLocation(cluster[i]).second);
    }
}
void ClusterProblem::initializeCost(const iProblem& mainProblem)
{
    cost = new Cost(Location);
    for (int i=0; i<Location; i++) {
		cost->setHoldingCost(i,mainProblem.getCost().getHoldingCost()[cluster[i]]);
        cost->setSalesCost(i,mainProblem.getCost().getSalesCost()[cluster[i]]);
        cost->setTransshipmentCost(i,mainProblem.getCost().getTransshipmentCost()[cluster[i]]);

    }
}


void ClusterProblem::initializeCoordinates(const iProblem& mainProblem)
{
    for (int i=0; i<Location; i++) {
        cost->setXCoordinates(i,mainProblem.getCost().getXCoordinates(cluster[i]));
        cost->setYCoordinates(i,mainProblem.getCost().getYCoordinates(cluster[i]));
    }
}

void ClusterProblem::outputInitialData(const string& fileName)const
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(),ios::app);
    outputFile<<"Number of locations: "<<Location<<'\n';
    outputFile<<"Time Horizon: "<<Time<<'\n';
    for (int i=0; i<Location; i++) {
        outputFile<<"Location: "<<cluster[i]<<'\n';
        outputFile<<"Initial IL: "<<state->getElement(i)<<'\n';
        outputFile<<"Holding cost: "<<cost->getHoldingCost()[i]<<'\n';
        outputFile<<"Transshipment cost: "<<cost->getTransshipmentCost()[i]<<'\n';
        outputFile<<"Sales cost: "<<cost->getSalesCost()[i]<<'\n';
        outputFile<<"Mean demand: "<<demand->getDemandParametersForLocation(i).first<<'\n';
        for (int j=0; j<Location; j++) {
            outputFile<<"Distance from location "<<cluster[i]<<" to location "<<cluster[j]<<" : "<<cost->getDistance(i,j)<<'\n';
        }
    } 
    outputFile.close();
}

