//
//  DP_Solver.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_DP_Solver_h
#define TransshipmentProject_DP_Solver_h

//#include "ProblemMethods.h"

#include "types.h"
#include "array.h"
#include "ValueFunction.h"
#include "Hashing.h"

class State;
class Cost;
//class Actions;
class Demand;
//class Hashing;
//class ValueFunction;
class iProblem;
class iDistribution;

class DP_Solver
{
public:
    DP_Solver(){initializeValueFunction();initializeHashFunction();}
    ~DP_Solver();
    
    void solve(iProblem& problem, const std::string& f_fileName);
    
    ValueFunction& getValueFunction() const;
    
    Hashing& getHashFunction() const;
    
protected:
    
    ValueFunction *valueFunction;
    Hashing *hashFunction;
    
    void initializeValueFunction();
    
    void initializeHashFunction();

    
    void getLookUpTable(const int& location, const int& time, const int& lifeTime, const State& initialState,const Cost& cost,Demand& demand);
    
    //type=1
    //void getLookUpTablePost(const int& location, const int& time, const State& initialState,const Cost& cost,Demand& demand);
    
    //type!=0 & type!=1
    void getLookUpTable(const std::string& f_fileName);
    
    
    void outputLookUpTable(const std::string& f_fileName);
    
    virtual float getMaxProfit(const int& t, const int& time, const State& state, Demand& demand, const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution) const=0;
    //float getMaxCostPost(const int& t,const State& state, const Cost& cost) const;

    

    //-------------------------help methods to check concavity----------------------------
    
    void addToFile(const std::string& fileName,const int& x, const int& y, const float& z);
    
    void createGraph(const int& size1, const int& size2,const std::string& fileName, const int& time);
    
    bool checkConcavity(const matrix_t& matrix);
    bool checkConcavity(const vecMatrixFloat& matrix);
    
    vector_t getMaxValueForSumIL(const State& state);
    
    void getParametersForDemandGeneration(State& maxBorder, int& size_inventoryLevels, const int& maxValue);
    
    
    
};


#endif
