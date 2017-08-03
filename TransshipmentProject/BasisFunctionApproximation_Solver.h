//
//  BasisFunctionApproximation_Solver.h
//  TransshipmentProject
//
//  Created by Olga on 13/03/15.
//
//

#ifndef __TransshipmentProject__BasisFunctionApproximation_Solver__
#define __TransshipmentProject__BasisFunctionApproximation_Solver__

#include "BFValueFunction.h"
#include <boost/shared_ptr.hpp>

class iProblem;
class State;
class BFValueFunction;
class iDistribution;
class PiecewiseLinearFunctionSet;
class Cost;
class Demand;

class BasisFunctionApproximation_Solver
{
public:
    BasisFunctionApproximation_Solver(const  boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction, const int& iterationLearning,const int& expectationLearning){initializePiecewiseAppVF(NoTransshipmentFunction); iterations=iterationLearning;initializeValueFunction();demandNumber=expectationLearning;setThreadNumber(iterationLearning);}
    ~BasisFunctionApproximation_Solver();
    
    void solve(iProblem& problem);
    
    BFValueFunction& getBFValueFunction() const;
    
    
private:
     boost::shared_ptr<PiecewiseLinearFunctionSet> F;
    BFValueFunction *valueFunction;
    int iterations;
    int demandNumber;
    int threadNumber;
    
    void initializeValueFunction();
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& piecewiseVF);
    
    void generateSamplePath(iProblem& problem,std::vector<std::vector<State> >& path,const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    float getExpectedProfitBasedOnApproximation(iProblem& problem,const int& currentPeriod, const State& currentState,const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    //int generateBorder(const State& initialState, const int& currentPeriod,const std::vector<boost::shared_ptr<iDistribution> >& distribution, const int& currentLocation);
    
    float calculateValueForStateBasedOnApproximation(const int& currentPeriod, const State& state);
    
    float getExpectedProfitNextPeriodsWithoutTransshipments(const int& location,const int& currentPeriod, const State& currentState);
    
    float getExpectedProfitWithCurrentTransshipment(const Cost& cost,const int& currentPeriod, const State& currentState);
    
    void setThreadNumber(const int& replicationNumber);
    
    void getExpectedProfitNextPeriodForFixedDemand(const int& location, const Cost& cost, Demand& demand,const int& currentPeriod, const State& currentState,const std::vector<boost::shared_ptr<iDistribution> >& distribution, float& expectedProfit);
    
};

#endif /* defined(__TransshipmentProject__BasisFunctionApproximation_Solver__) */
