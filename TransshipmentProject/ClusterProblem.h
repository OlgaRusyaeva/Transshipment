//
//  ClusterProblem.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ClusterProblem_h
#define TransshipmentProject_ClusterProblem_h

#include "iProblem.h"
#include "types.h"

class ClusterProblem: public iProblem
{
public:
    ClusterProblem(const iProblem& mainProblem,const vector_t& clusterVector);
    
    ~ClusterProblem();
    
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
    
    vector_t cluster;
    State *state;
    Demand *demand;
    Cost *cost;
    //Actions *actions;
    
    void outputInitialData(const std::string& fileName)const;
    
    void initialize(const iProblem& mainProblem);
    
    void initializeState(const iProblem& mainProblem);
    
    void initializeDemand(const iProblem& mainProblem);
    
    void initializeCost(const iProblem& mainProblem);
    
    void initializeCoordinates(const iProblem& mainProblem);
    
    
};


#endif
