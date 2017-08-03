//
//  AggregatedProblem.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_AggregatedProblem_h
#define TransshipmentProject_AggregatedProblem_h

#include "iProblem.h"
#include "types.h"

class Cluster;

class AggregatedProblem: public iProblem
{
public:
    AggregatedProblem(const iProblem& mainProblem,const Cluster& cluster);
    
    ~AggregatedProblem();
    
    const State& getState() const;
    
    const Demand& getDemand() const;
    
    const int getStateDimension() const;
    
    const int getTimeHorizon()const;
    
    //const Actions& getDistances() const;
    
    const Cost& getCost() const;
    
    const vector_t& getActionPeriods() const; 
    
    
    void outputInput(const std::string& fileName)const;
    
    
private:
    int Time, Location;
    
    vector_t actionPeriods;
    
    State *state;
    Demand *demand;
    Cost *cost;
    //Actions *actions;
    
    void outputInitialData(const std::string& fileName)const;
    
    void initialize(const iProblem& mainProblem,const Cluster& cluster);
    
    void initializeState(const iProblem& mainProblem,const Cluster& cluster);
    
    void initializeDemand(const iProblem& mainProblem,const Cluster& cluster);
    
    void initializeCost(const iProblem& mainProblem,const Cluster& cluster);
    
    void initializeCoordinates(const iProblem& mainProblem,const Cluster& cluster);
    
    //float distance(const int& clusterA, const int& clusterB)const;
    
    
};


#endif
