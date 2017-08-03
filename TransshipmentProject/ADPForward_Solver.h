//
//  ADPCluster_Solver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 3/5/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ADPCluster_Solver_h
#define TransshipmentProject_ADPCluster_Solver_h

#include "types.h"
#include "array.h"

class iDistribution;
class Cost;
//class Actions;
class Demand;
class Hashing;
class ValueFunction;
class iProblem;
class State;
class Cluster;


class ADPForward_Solver
{
public:
    ADPForward_Solver(){alpha=4;demandNumber=100;epsilon=0.1;initializeValueFunction();initializeHashFunction();}
    ~ADPForward_Solver();
    
    void solve(iProblem& problem,const int& iterationNumber, const std::string& f_fileName);
    
    ValueFunction& getValueFunction() const;
    
    Hashing& getHashFunction() const;
    
protected:
    float alpha;
    int demandNumber;
    float epsilon;
    
    ValueFunction *valueFunction;
    Hashing *hashFunction;
    
    void initializeValueFunction();
    
    void initializeHashFunction();

    
    void getLookUpTable(const int& location, const int& time, const State& initialState,const Cost& cost, Demand& demand,const int& iterationNumber);
    
    //void getLookUpTable(ValueFunction& valueFunction,const std::string& f_fileName);
    
    void outputLookUpTable(const std::string& f_fileName);
    
	//void outputClusters(const Actions& distance,Cluster& cluster);
    
    void updateValueFunction(const float& cost, const size_t& initialStateHash, const int& t, const float& alpha_n);
    
	virtual float getCostForActions(State& state,const int& currentPeriod,const Cost& cost, const int& time, const arrays::Array2D<float>& possibleDemands)const=0;
    
    void getDemandSample(arrays::Array2D<float>& possibleDemands, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
};

#endif
