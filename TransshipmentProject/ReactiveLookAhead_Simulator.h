//
//  ReactiveLookAhead_Simulator.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/17/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_ReactiveLookAhead_Simulator_h
#define TransshipmentProject_ReactiveLookAhead_Simulator_h

#include "types.h"
#include "Simulator.h"

class State;
class Cost;
class Actions;
class Demand;
class iDistribution;

class ReactiveLookAhead_Simulator: public Simulator
{
public:
    //ReactiveLookAhead_Simulator(const int& N): Simulator(N){}
    ~ReactiveLookAhead_Simulator(){}
    
    //void simulator(const int& location, const int& time, const State& initialState,const Cost& cost, const Actions& distanceMatrix, Demand& demand, row_t& solution);
    
     float getTransshipmentCost(const int& location, const int& time, const int& currentPeriod, State& state, const Cost& cost,std::vector<boost::shared_ptr<iDistribution> > distribution, Actions& maxTransshipmentMatrix);
    
    
private:
    
    
    //float gettransshipmentCostByReactiveLookAhead(const int& location, const int& time, State& state, const Cost& cost, const Actions& distanceMatrix,std::vector<boost::shared_ptr<iDistribution> > distribution, const int& restTime,Actions& maxTransshipmentMatrix);
    
    vector_t getNonStockOutLocations(const State& state);
    
    //change calculations of future profit difference
    float getFutureProfit(const int& j, const int& i,const Cost& cost, const State& state, std::vector<boost::shared_ptr<iDistribution> > distribution, int& amountToTransship, const int& restTime);
    
    float getAllPeriodsProfit(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr,const int& time);
    
    float getOnePeriodProfit(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr);
    
};


#endif
