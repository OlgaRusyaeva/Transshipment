//
//  ADPCluster_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ADPCluster_Simulator_h
#define TransshipmentProject_ADPCluster_Simulator_h


#include "types.h"
#include "Simulator.h"
#include "array.h"

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;
class Hashing;
class ValueFunction;
class Cluster;
class iProblem;


class ADPCluster_Simulator: public Simulator
{
public:
    ADPCluster_Simulator(const int& numberOfIterations, const int& seedNumber,const std::vector<ValueFunction>& valueFunction, const std::vector<Hashing>& hashFunction, const Cluster& cluster):Simulator(numberOfIterations,seedNumber){demandNumber=100;initializeValueFunction(valueFunction);initializeHashFunction(hashFunction);initializeCluster(cluster);}
    ~ADPCluster_Simulator();
    
    float getTransshipmentCost(iProblem& problem,const int& currentPeriod, State& currentState, const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution, Actions& maxTransshipmentMatrix);
    
    
private:
    int demandNumber;
    std::vector<ValueFunction*> VF;
    std::vector<Hashing*> HF;
    
    Cluster* CL;
    
    void initializeValueFunction(const std::vector<ValueFunction>& valueFunction);
    
    void initializeHashFunction(const std::vector<Hashing>& hashFunction);
    
    void initializeCluster(const Cluster& cluster);
    
	Actions getActionsBetweenClusters(const int& time, State& state,const int& currentPeriod,const Cost& cost,const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    State getAggregatedState(const State& state);
    
    Actions transformActionsBetweenClusters(const Actions& transshipment, const State& currentState, const Cost& cost,const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& restTime);
    
    void makeTransshipmentsBetweenClusters(Actions& transshipments, const vector_t& locationsInClusterA, const vector_t& locationsInClusterB, const int& amountToTransship, const Cost& cost,const std::vector<boost::shared_ptr<iDistribution> >& distribution, State& state, const int& restTime);
    
    void getDemandSample(arrays::Array2D<float>& possibleDemands, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    Actions getActionsInsideCluster(const int& time, State& state,const int& currentPeriod,const Cost& cost, const int& clusterID,const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
	State getStateForCluster(const State& state, const int& clusterID);
    
	Actions transformActionsInsideCluster(const Actions& clusteredActions, const State& currentState, const int& clusterID, const Cost& cost);
    
    void getDemandForCluster(const State& state,arrays::Array2D<float>& demandMatrix);
    
    void getDemandSize(int& height, const State& state);
    
    Demand transformClusterDemand(const Demand& clusterDemand, const int& location, const int& clusterID);
    
    State transformClusterState(const State& clusterState, const int& location, const int& clusterID);
    
    
};





#endif
