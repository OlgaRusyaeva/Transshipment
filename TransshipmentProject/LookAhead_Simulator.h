//
//  LookAhead_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_LookAhead_Simulator_h
#define TransshipmentProject_LookAhead_Simulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;

class LookAhead_Simulator: public Simulator
{
public:
   // LookAhead_Simulator(const int& N): Simulator(N){}
    ~LookAhead_Simulator(){}
    
    //void simulator(const int& location, const int& time, const State& initialState,const Cost& cost, const Actions& distanceMatrix, Demand& demand, row_t& solution);
    
     float getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost, std::vector<boost::shared_ptr<iDistribution> > distribution, Actions& maxTransshipmentMatrix);
    
    
private:
    
    
    //float gettransshipmentCostByLookAhead(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,std::vector<boost::shared_ptr<iDistribution> > distribution, const int& currentPeriod,Actions& maxTransshipmentMatrix);
    
    vector_t getNonStockOutLocations(const State& state);
    
    void setOnePlusState(const State& state,State& stateOnePlus);
    
    void setOneMinusState(const State& state,State& stateOneMinus,const vector_t& left_over);
    
    row_t getTransshipmentProfitOnePeriod(const Cost& cost, std::vector<boost::shared_ptr<iDistribution> > distribution, const State& state);
    
    row_t getTransshipmentProfitTwoPeriods(const Cost& cost, std::vector<boost::shared_ptr<iDistribution> > distribution, const State& state);
    
    int getLowestDecreaseInProfit(const row_t& profitNo, const row_t& profitLess, const State& state,const vector_t& left_over);
    
    int getHighestIncreaseInProfit(const row_t& profitNo, const row_t& profitMore, const State& state);
    
    float getOnePeriodProfit(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr);
    
};



#endif
