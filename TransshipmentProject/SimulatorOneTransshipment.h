//
//  SimulatorOneTransshipment.h
//  TransshipmentProject
//
//  Created by Olga on 23/02/15.
//
//

#ifndef __TransshipmentProject__SimulatorOneTransshipment__
#define __TransshipmentProject__SimulatorOneTransshipment__

#include"types.h"
#include "PiecewiseLinearFunctionSet.h"
#include <boost/shared_ptr.hpp>

class iProblem;
class Profit;
class State;
class Actions;
class Demand;
class Cost;
class iDistribution;

class SimulatorOneTransshipment
{
public:
    
    SimulatorOneTransshipment(const int& iterations, const int& seedNumber,const  boost::shared_ptr<PiecewiseLinearFunctionSet>& NoTransshipmentFunction){seed=seedNumber;iterationNumber=iterations;initializePiecewiseAppVF(NoTransshipmentFunction);}
    
    ~SimulatorOneTransshipment();
    
    void simulate(iProblem& problem, Profit& profit);
    
    float getTransshipmentCost(const Cost& cost,const int& currentPeriod, State& currentState, Actions& maxTransshipmentMatrix);
    
   virtual bool timeToTransship(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration)=0;
    
	float getExpectedProfitNextPeriodsWithoutTransshipments(const int& location,const int& currentPeriod, const State& currentState);
    
    float getExpectedProfitWithCurrentTransshipment(const Cost& cost,const int& currentPeriod, const State& currentState, bool& isZero);
    
protected:
    int seed;
    int iterationNumber;
     boost::shared_ptr<PiecewiseLinearFunctionSet> F;
    //get all Locations that out of stock after demand satisfaction
    vector_t getStockOutLocations(const State& initialState);
    
    void initializePiecewiseAppVF(const boost::shared_ptr<PiecewiseLinearFunctionSet>& piecewiseVF);
    
    //float getExpectedProfitNextPeriods(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    //float getExpectedProfitNextPeriod(iProblem& problem,const int& currentPeriod, const State& currentState,const int& currentIteration, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
    //initialize transshipment matrix by zero
    matrixInt_t initializeTransshipmentMatrix(const int& size_row, const int& size_col);
    
    void printMatrix(const int& limit1,const int& limit2, const matrixInt_t& M);
    
    //void generateDemand(const int& location, const int& time, matrixInt_t& demandMatrix, const int& currentIteration, const int& replicationNumber, const std::vector<boost::shared_ptr<iDistribution> >& distribution);
    
};


#endif /* defined(__TransshipmentProject__SimulatorOneTransshipment__) */
