//
//  LP_optimizer.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 12/17/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_LP_optimizer_h
#define TransshipmentProject_LP_optimizer_h

#include "types.h"
#include "gurobi_c++.h"
#include "PiecewiseLinearFunctionSet.h"
#include "PLValueFunction.h"
#include <boost/shared_ptr.hpp>

class State;
class Cost;
class Graph;
class LP_solution;
class LeadTimes;


class LP_optimizer 
{
public:
    LP_optimizer(){}
    ~LP_optimizer()
    {
        delete[] lhsObj;
        delete[] vars;
        delete[] constr;
        delete model;
        delete env;
    }
    
    LP_solution optimize(const State& currentState,  const int& lifePeriod, const Cost& cost,const boost::shared_ptr<PiecewiseLinearFunctionSet>& PL, const int& t);
    
    LP_solution optimize(const State& currentState,  const Cost& cost,const boost::shared_ptr<PLValueFunction>& PL, const int& currentPeriod, const LeadTimes& leadTimes,const int& limit, const bool& flag);
    
    float optimalObjectiveFunction();
    
    bool checkIfOptimalBasicDegenerate();
    
    void getTrueShadowPrices(LP_solution& currentSolution, const int& L, const int& limit, const LeadTimes& leadTimes);
    
    void getShadowPricesAsDualVariables(LP_solution& solution);
    
    
    
private:

    int NumVar1;
    int NumVar2;
    int NumVar3;
    
    vector_t var2_amount;
    
    int NumVars;
    
    int NumConstr;
    
    GRBEnv* env;
    GRBModel* model;
    GRBVar* vars;
    GRBConstr* constr;
    GRBLinExpr* lhsObj;
    
    void getBasicVariables(vector_t& basic);
    
    row_t leastCostFlowAugmentingPath(const bool& flag, Graph& g);
    
    row_t initializeWeights();
    
    vector_t initializeCapacities(const int& L, const int& limit);
    
    vector_t initializeFlow();
    
    void transform(row_t& pi, const int& L, const int& limit);
    
    //void printShadowPrices(const LP_solution& solution);
    
    //void printDuals();


    
    
};

#endif
