//
//  MyopicMethods.h
//  TransshipmentProject
//
//  Created by Olga Rusyaeva on 2/18/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//

#ifndef TransshipmentProject_MyopicMethods_h
#define TransshipmentProject_MyopicMethods_h

#include <boost/shared_ptr.hpp>
#include "types.h"

class iDistribution;
class State;
class Cost;

namespace myopicMethods
{
    float expectedValuePositivePart(const int& time,const boost::shared_ptr<iDistribution>& distr,const int& stateValue);
    
    //float expectedValuePositivePart(const boost::shared_ptr<iDistribution>& distr,const int& stateValue);
    
    vector_t getNonStockOutLocations(const State& state,const vector_t& source);
    
    void setOnePlusState(const State& state,State& stateOnePlus);
    
    void setOneMinusState(const State& state,State& stateOneMinus,const vector_t& left_over);
    
    //row_t getTransshipmentProfitOnePeriod(const Cost& cost, std::vector<boost::shared_ptr<iDistribution> > distribution, const State& state);
    
    int getLowestDecreaseInProfit(const row_t& profitNo, const row_t& profitLess,const vector_t& left_over);
    
    int getHighestIncreaseInProfit(const row_t& profitNo, const row_t& profitMore, const vector_t& destination);
    
    //float getOnePeriodProfit(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr);
    
    float getExpectedProfitRestPeriods(const float& p, const float& h, const int& IL, const boost::shared_ptr<iDistribution>& distr, const int& restTime);
    
    row_t getTransshipmentProfitRestPeriods(const Cost& cost, const std::vector<boost::shared_ptr<iDistribution> >& distribution, const State& state,const int& restTime);
    
    //float pdfSumIndependetRandomVariables(const int& x,const boost::shared_ptr<iDistribution>& distr,int amount);
    
    //float cdfSumIndependetRandomVariables(const int& amount,const boost::shared_ptr<iDistribution>& distr,const int& x);
    
    float onePeriodProfit(const int& currentIL, const int& nextIL, const float& p, const float& h);
    
    vector_t getLocationsForTransshipments(const State& state);
};

#endif
