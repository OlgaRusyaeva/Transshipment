//
//  AggregatedProblem.cpp
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#include "AggregatedProblem.h"
#include "State.h"
#include "Demand.h"
#include "Cost.h"
#include "Output.h"
#include "Cluster.h"

using namespace std;

AggregatedProblem::AggregatedProblem(const iProblem& mainProblem,const Cluster& cluster)
{
    initialize(mainProblem,cluster);
    initializeState(mainProblem,cluster);
    initializeDemand(mainProblem,cluster);
    initializeCost(mainProblem,cluster);
    initializeCoordinates(mainProblem,cluster);
}

AggregatedProblem::~AggregatedProblem()
{
    delete state;
    delete demand;
    delete cost;
    //delete actions;
}
const State& AggregatedProblem::getState() const
{
    return *state;
}

const Demand& AggregatedProblem::getDemand() const
{
    return *demand;
}


const int AggregatedProblem::getStateDimension() const
{
    return Location;
}

const int AggregatedProblem::getTimeHorizon()const
{
    return Time;
}

/*const Actions& AggregatedProblem::getDistances() const
{
    return *actions;
}*/
const Cost& AggregatedProblem::getCost() const
{
    return *cost;
}

void AggregatedProblem::outputInput(const string& fileName)const
{
    outputInitialData(fileName); 
}



void AggregatedProblem::initialize(const iProblem& mainProblem,const Cluster& cluster)
{
    Time=mainProblem.getTimeHorizon();
    Location=cluster.getNumberOfClusters();
}

void AggregatedProblem::initializeState(const iProblem& mainProblem,const Cluster& cluster)
{
    state = new State(Location);
    for (int i=0; i<Location; i++) {
        int sumIL=0;
        for (int j=0; j<cluster.getNumberLocationsInCluster(i); j++) {
            sumIL+=mainProblem.getState().getElement(cluster.getLocationsForCluster(i)[j]);
        }
        state->setElement(i,sumIL);
    }
}

void AggregatedProblem::initializeDemand(const iProblem& mainProblem,const Cluster& cluster)
{
    demand = new Demand(Location);
    //as the demand each period independent 
    //otherwise can't sum variance
    //should change demand generation if the distibution is not poisson
    for (int i=0; i<Location; i++) {
        float sumMean=0;
        float sumVar=0;
        for (int j=0; j<cluster.getNumberLocationsInCluster(i); j++) {
            sumMean+=mainProblem.getDemand().getDemandParametersForLocation(cluster.getLocationsForCluster(i)[j]).first;
            sumVar+=mainProblem.getDemand().getDemandParametersForLocation(cluster.getLocationsForCluster(i)[j]).second;
        }
        demand->setDemandParameters(i,sumMean,sumVar);
    }
}
void AggregatedProblem::initializeCost(const iProblem& mainProblem,const Cluster& cluster)
{
    cost = new Cost(Location);
    for (int i=0; i<Location; i++) {
        float sumHoldingCost=0;
        float sumSalesPrice=0;
        float sumTransshipmentCost=0;
        for (int j=0; j<cluster.getNumberLocationsInCluster(i); j++) {
            sumHoldingCost+=mainProblem.getCost().getHoldingCost()[cluster.getLocationsForCluster(i)[j]];
            sumSalesPrice+=mainProblem.getCost().getSalesCost()[cluster.getLocationsForCluster(i)[j]];
            sumTransshipmentCost+=mainProblem.getCost().getTransshipmentCost()[cluster.getLocationsForCluster(i)[j]];
        }
		cost->setHoldingCost(i,(sumHoldingCost/cluster.getNumberLocationsInCluster(i)));
        cost->setSalesCost(i,(sumSalesPrice/cluster.getNumberLocationsInCluster(i)));
        cost->setTransshipmentCost(i,(sumTransshipmentCost/cluster.getNumberLocationsInCluster(i)));
        
    }
}


void AggregatedProblem::initializeCoordinates(const iProblem& mainProblem,const Cluster& cluster)
{
    for (int i=0; i<Location; i++) {
        float xCoordinate=0;
        float yCoordinate=0;
        for (int j=0; j<cluster.getNumberLocationsInCluster(i); j++) {
            xCoordinate+=mainProblem.getCost().getXCoordinates(cluster.getLocationsForCluster(i)[j]);
            yCoordinate+=mainProblem.getCost().getYCoordinates(cluster.getLocationsForCluster(i)[j]);
        }
        cost->setXCoordinates(i,(xCoordinate/cluster.getNumberLocationsInCluster(i)));
        cost->setYCoordinates(i,(yCoordinate/cluster.getNumberLocationsInCluster(i)));
    }
    
}

void AggregatedProblem::outputInitialData(const string& fileName)const
{
    ofstream outputFile;
    outputFile.open(fileName.c_str(),ios::app);
    outputFile<<"Number of locations: "<<Location<<'\n';
    outputFile<<"Time Horizon: "<<Time<<'\n';
    for (int i=0; i<Location; i++) {
        outputFile<<"Location: "<<i<<'\n';
        outputFile<<"Initial IL: "<<state->getElement(i)<<'\n';
        outputFile<<"Holding cost: "<<cost->getHoldingCost()[i]<<'\n';
        outputFile<<"Transshipment cost: "<<cost->getTransshipmentCost()[i]<<'\n';
        outputFile<<"Sales cost: "<<cost->getSalesCost()[i]<<'\n';
        outputFile<<"Mean demand: "<<demand->getDemandParametersForLocation(i).first<<'\n';
        for (int j=0; j<Location; j++) {
            outputFile<<"Distance from cluster "<<i<<" to cluster "<<j<<" : "<<cost->getDistance(i,j)<<'\n';
        }
    } 
    outputFile.close();
}
                                 
/*float AggregatedProblem::distance(const int& clusterA, const int& clusterB)const
{
    return sqrtf(pow(((float)(actions->getXCoordinates(clusterA)-actions->getXCoordinates(clusterB))), 2)+pow(((float)(actions->getYCoordinates(clusterA)-actions->getYCoordinates(clusterB))), 2)); 
}*/

